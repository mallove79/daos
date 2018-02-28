/**
 * (C) Copyright 2016 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * GOVERNMENT LICENSE RIGHTS-OPEN SOURCE SOFTWARE
 * The Government's rights to use, modify, reproduce, release, perform, display,
 * or disclose this software are subject to the terms of the Apache License as
 * provided in Contract No. B609815.
 * Any reproduction of computer software, computer software documentation, or
 * portions thereof marked with this legend must also reproduce the markings.
 */
/*
 * This file is part of common DAOS library.
 *
 * common/tse.c
 *
 * DAOS client will use scheduler/task to manage the asynchronous tasks.
 * Tasks will be attached to one scheduler, when scheduler is executed,
 * it will walk through the task list of the scheduler and pick up those
 * ready tasks to executed.
 */
#define DDSUBSYS	DDFAC(client)

#include <stdint.h>
#include <pthread.h>
#include <daos/common.h>
#include <daos/tse.h>
#include "tse_internal.h"

struct tse_task_link {
	d_list_t		 tl_link;
	tse_task_t		*tl_task;
};

static void tse_sched_decref(struct tse_sched_private *dsp);

int
tse_sched_init(tse_sched_t *sched, tse_sched_comp_cb_t comp_cb,
	       void *udata)
{
	struct tse_sched_private *dsp = tse_sched2priv(sched);
	int rc;

	D_CASSERT(sizeof(sched->ds_private) >= sizeof(*dsp));

	memset(sched, 0, sizeof(*sched));

	D_INIT_LIST_HEAD(&dsp->dsp_init_list);
	D_INIT_LIST_HEAD(&dsp->dsp_running_list);
	D_INIT_LIST_HEAD(&dsp->dsp_complete_list);
	D_INIT_LIST_HEAD(&dsp->dsp_comp_cb_list);

	dsp->dsp_refcount = 1;
	dsp->dsp_inflight = 0;
	pthread_mutex_init(&dsp->dsp_lock, NULL);

	if (comp_cb != NULL) {
		rc = tse_sched_register_comp_cb(sched, comp_cb, udata);
		if (rc != 0)
			return rc;
	}

	sched->ds_udata = udata;
	sched->ds_result = 0;

	return 0;
}

static inline uint32_t
tse_task_buf_size(int size)
{
	return (size + 7) & ~0x7;
}

/*
 * MSC - I changed this to be just a single buffer and not as before where it
 * keeps giving an addition pointer to the big pre-allcoated buffer. previous
 * way doesn't work well for public use.
 * We should make this simpler now and more generic as the comment below.
 */
void *
tse_task_buf_embedded(tse_task_t *task, int size)
{
	struct tse_task_private *dtp = tse_task2priv(task);

	/** Let's assume dtp_buf is always enough at the moment */
	/** MSC - should malloc if size requested is bigger */
	D__ASSERTF(tse_task_buf_size(size) <= sizeof(dtp->dtp_buf),
		  "req size %u all size %lu\n",
		  tse_task_buf_size(size), sizeof(dtp->dtp_buf));
	return (void *)dtp->dtp_buf;
}

void *
tse_task_get_priv(tse_task_t *task)
{
	struct tse_task_private *dtp = tse_task2priv(task);

	return dtp->dtp_priv;
}

void *
tse_task_set_priv(tse_task_t *task, void *priv)
{
	struct tse_task_private *dtp = tse_task2priv(task);
	void			*old = dtp->dtp_priv;

	dtp->dtp_priv = priv;
	return old;
}

tse_sched_t *
tse_task2sched(tse_task_t *task)
{
	struct tse_sched_private	*sched_priv;

	sched_priv = tse_task2priv(task)->dtp_sched;
	return tse_priv2sched(sched_priv);
}

static void
tse_task_addref_locked(struct tse_task_private *dtp)
{
	dtp->dtp_refcnt++;
}

static bool
tse_task_decref_locked(struct tse_task_private *dtp)
{
	D__ASSERT(dtp->dtp_refcnt > 0);
	dtp->dtp_refcnt--;
	return dtp->dtp_refcnt == 0;
}

void
tse_task_addref(tse_task_t *task)
{
	struct tse_task_private  *dtp = tse_task2priv(task);
	struct tse_sched_private *dsp = dtp->dtp_sched;

	D__ASSERT(dsp != NULL);

	pthread_mutex_lock(&dsp->dsp_lock);
	tse_task_addref_locked(dtp);
	pthread_mutex_unlock(&dsp->dsp_lock);
}

static inline void
tse_task_ret_list_cleanup(tse_task_t *task)
{
	struct tse_task_private  *dtp = tse_task2priv(task);

	while (!d_list_empty(&dtp->dtp_ret_list)) {
		struct tse_task_link *result;

		result = d_list_entry(dtp->dtp_ret_list.next,
				      struct tse_task_link, tl_link);
		d_list_del(&result->tl_link);
		tse_task_decref(result->tl_task);
		D__FREE_PTR(result);
	}
}

void
tse_task_decref(tse_task_t *task)
{
	struct tse_task_private  *dtp = tse_task2priv(task);
	struct tse_sched_private *dsp = dtp->dtp_sched;
	bool			   zombie;

	D__ASSERT(dsp != NULL);
	pthread_mutex_lock(&dsp->dsp_lock);
	zombie = tse_task_decref_locked(dtp);
	pthread_mutex_unlock(&dsp->dsp_lock);
	if (!zombie)
		return;

	tse_task_ret_list_cleanup(task);
	D__ASSERT(d_list_empty(&dtp->dtp_dep_list));

	/*
	 * MSC - since we require user to allocate task, maybe we should have
	 * user also free it. This now requires task to be on the heap all the
	 * time.
	 */
	D__FREE_PTR(task);
}

void
tse_sched_fini(tse_sched_t *sched)
{
	struct tse_sched_private *dsp = tse_sched2priv(sched);

	D__ASSERT(dsp->dsp_inflight == 0);
	D__ASSERT(d_list_empty(&dsp->dsp_init_list));
	D__ASSERT(d_list_empty(&dsp->dsp_running_list));
	D__ASSERT(d_list_empty(&dsp->dsp_complete_list));
	pthread_mutex_destroy(&dsp->dsp_lock);
}

static inline void
tse_sched_addref_locked(struct tse_sched_private *dsp)
{
	dsp->dsp_refcount++;
}

static void
tse_sched_decref(struct tse_sched_private *dsp)
{
	bool	finalize;

	pthread_mutex_lock(&dsp->dsp_lock);

	D__ASSERT(dsp->dsp_refcount > 0);
	dsp->dsp_refcount--;
	finalize = dsp->dsp_refcount == 0;

	pthread_mutex_unlock(&dsp->dsp_lock);

	if (finalize)
		tse_sched_fini(tse_priv2sched(dsp));
}

int
tse_sched_register_comp_cb(tse_sched_t *sched,
			   tse_sched_comp_cb_t comp_cb, void *arg)
{
	struct tse_sched_private	*dsp = tse_sched2priv(sched);
	struct tse_sched_comp		*dsc;

	D__ALLOC_PTR(dsc);
	if (dsc == NULL)
		return -DER_NOMEM;

	dsc->dsc_comp_cb = comp_cb;
	dsc->dsc_arg = arg;

	pthread_mutex_lock(&dsp->dsp_lock);
	d_list_add(&dsc->dsc_list, &dsp->dsp_comp_cb_list);
	pthread_mutex_unlock(&dsp->dsp_lock);
	return 0;
}

/** MSC - we probably need just 1 completion cb instead of a list */
static int
tse_sched_complete_cb(tse_sched_t *sched)
{
	struct tse_sched_comp		*dsc;
	struct tse_sched_comp		*tmp;
	struct tse_sched_private	*dsp = tse_sched2priv(sched);
	int				rc;

	d_list_for_each_entry_safe(dsc, tmp, &dsp->dsp_comp_cb_list, dsc_list) {
		d_list_del(&dsc->dsc_list);
		rc = dsc->dsc_comp_cb(dsc->dsc_arg, sched->ds_result);
		if (sched->ds_result == 0)
			sched->ds_result = rc;
		D__FREE_PTR(dsc);
	}
	return 0;
}

/* Mark the tasks to complete */
static void
tse_task_complete_locked(struct tse_task_private *dtp,
			 struct tse_sched_private *dsp)
{
	if (dtp->dtp_completed)
		return;

	D__ASSERT(dtp->dtp_running);
	dtp->dtp_running = 0;
	dtp->dtp_completing = 0;
	dtp->dtp_completed = 1;
	d_list_move_tail(&dtp->dtp_list, &dsp->dsp_complete_list);
}

static int
register_cb(tse_task_t *task, bool is_comp, tse_task_cb_t cb,
	    void *arg, daos_size_t arg_size)
{
	struct tse_task_private *dtp = tse_task2priv(task);
	struct tse_task_cb *dtc;

	if (dtp->dtp_completed) {
		D__ERROR("Can't add a callback for a completed task\n");
		return -DER_NO_PERM;
	}

	D__ALLOC(dtc, sizeof(*dtc) + arg_size);
	if (dtc == NULL)
		return -DER_NOMEM;

	dtc->dtc_arg_size = arg_size;
	dtc->dtc_cb = cb;
	if (arg)
		memcpy(dtc->dtc_arg, arg, arg_size);

	D__ASSERT(dtp->dtp_sched != NULL);

	pthread_mutex_lock(&dtp->dtp_sched->dsp_lock);
	if (is_comp)
		d_list_add(&dtc->dtc_list, &dtp->dtp_comp_cb_list);
	else /** MSC - don't see a need for more than 1 prep cb */
		d_list_add_tail(&dtc->dtc_list, &dtp->dtp_prep_cb_list);

	pthread_mutex_unlock(&dtp->dtp_sched->dsp_lock);

	return 0;
}

int
tse_task_register_comp_cb(tse_task_t *task, tse_task_cb_t comp_cb,
			  void *arg, daos_size_t arg_size)
{
	if (comp_cb)
		register_cb(task, true, comp_cb, arg, arg_size);

	return 0;
}

int
tse_task_register_cbs(tse_task_t *task, tse_task_cb_t prep_cb,
		      void *prep_data, daos_size_t prep_data_size,
		      tse_task_cb_t comp_cb, void *comp_data,
		      daos_size_t comp_data_size)
{
	if (prep_cb)
		register_cb(task, false, prep_cb, prep_data, prep_data_size);
	if (comp_cb)
		register_cb(task, true, comp_cb, comp_data, comp_data_size);

	return 0;
}

/*
 * Execute the prep callback(s) of the task.
 */
static bool
tse_task_prep_callback(tse_task_t *task)
{
	struct tse_task_private	*dtp = tse_task2priv(task);
	struct tse_task_cb	*dtc;
	struct tse_task_cb	*tmp;
	int			 rc;

	d_list_for_each_entry_safe(dtc, tmp, &dtp->dtp_prep_cb_list, dtc_list) {
		d_list_del(&dtc->dtc_list);
		/** no need to call if task was completed in one of the cbs */
		if (!dtp->dtp_completed) {
			rc = dtc->dtc_cb(task, dtc->dtc_arg);
			if (task->dt_result == 0)
				task->dt_result = rc;
		}

		D__FREE(dtc, offsetof(struct tse_task_cb,
				     dtc_arg[dtc->dtc_arg_size]));

		/** Task was re-initialized; break */
		if (!dtp->dtp_running && !dtp->dtp_completing)
			return false;
	}

	return true;
}

/*
 * Execute the callback of the task and returns true if all CBs were executed
 * and non re-init the task. If the task is re-initialized by the user, it means
 * it's in-flight again, so we break at the current CB that re-initialized it,
 * and return false, meaning the task is not completed. All the remaining CBs
 * that haven't been executed remain attached, but the ones that have executed
 * already have been removed from the list at this point.
 */
static bool
tse_task_complete_callback(tse_task_t *task)
{
	struct tse_task_private	*dtp = tse_task2priv(task);
	struct tse_task_cb	*dtc;
	struct tse_task_cb	*tmp;

	d_list_for_each_entry_safe(dtc, tmp, &dtp->dtp_comp_cb_list, dtc_list) {
		int ret;

		d_list_del(&dtc->dtc_list);
		ret = dtc->dtc_cb(task, dtc->dtc_arg);
		if (task->dt_result == 0)
			task->dt_result = ret;

		D__FREE(dtc, offsetof(struct tse_task_cb,
				     dtc_arg[dtc->dtc_arg_size]));

		/** Task was re-initialized; break */
		if (!dtp->dtp_completing) {
			D__DEBUG(DB_TRACE, "re-init task %p\n", task);
			return false;
		}
	}

	return true;
}

/** Walk through the result task list and execute callback for each task. */
void
tse_task_result_process(tse_task_t *task, tse_task_cb_t callback, void *arg)
{
	struct tse_task_private	*dtp = tse_task2priv(task);
	struct tse_task_link	*result;

	d_list_for_each_entry(result, &dtp->dtp_ret_list, tl_link)
		callback(result->tl_task, arg);
}

/*
 * Process the task in the init list of the scheduler. This executes all the
 * body function of all tasks with no dependencies in the scheduler's init
 * list.
 */
static int
tse_sched_process_init(struct tse_sched_private *dsp)
{
	struct tse_task_private		*dtp;
	struct tse_task_private		*tmp;
	d_list_t			list;
	int				processed = 0;

	D_INIT_LIST_HEAD(&list);
	pthread_mutex_lock(&dsp->dsp_lock);
	d_list_for_each_entry_safe(dtp, tmp, &dsp->dsp_init_list, dtp_list) {
		if (dtp->dtp_dep_cnt == 0 || dsp->dsp_cancelling) {
			d_list_move_tail(&dtp->dtp_list, &list);
			dsp->dsp_inflight++;
		}
	}
	pthread_mutex_unlock(&dsp->dsp_lock);

	while (!d_list_empty(&list)) {
		tse_task_t *task;
		bool bumped = false;

		dtp = d_list_entry(list.next, struct tse_task_private,
				   dtp_list);

		task = tse_priv2task(dtp);

		pthread_mutex_lock(&dsp->dsp_lock);
		if (dsp->dsp_cancelling) {
			tse_task_complete_locked(dtp, dsp);
		} else {
			dtp->dtp_running = 1;
			d_list_move_tail(&dtp->dtp_list,
					 &dsp->dsp_running_list);
			/** +1 in case prep cb calls task_complete() */
			tse_task_addref_locked(dtp);
			bumped = true;
		}
		pthread_mutex_unlock(&dsp->dsp_lock);

		if (!dsp->dsp_cancelling) {
			/** if task is reinitialized in prep cb, skip over it */
			if (!tse_task_prep_callback(task)) {
				tse_task_decref(task);
				continue;
			}
			D__ASSERT(dtp->dtp_func != NULL);
			if (!dtp->dtp_completed)
				dtp->dtp_func(task);
		}
		if (bumped)
			tse_task_decref(task);

		processed++;
	}
	return processed;
}

/**
 * Check the task in the complete list, dependent task
 * status check, schedule status update etc. The task
 * will be moved to fini list after this
 **/
static int
tse_task_post_process(tse_task_t *task)
{
	struct tse_task_private  *dtp = tse_task2priv(task);
	struct tse_sched_private *dsp = dtp->dtp_sched;
	int rc = 0;

	D__ASSERT(dtp->dtp_completed == 1);

	/* set scheduler result */
	if (tse_priv2sched(dsp)->ds_result == 0)
		tse_priv2sched(dsp)->ds_result = task->dt_result;

	/* Check dependent list */
	pthread_mutex_lock(&dsp->dsp_lock);
	while (!d_list_empty(&dtp->dtp_dep_list)) {
		struct tse_task_link	*tlink;
		tse_task_t		*task_tmp;
		struct tse_task_private	*dtp_tmp;

		tlink = d_list_entry(dtp->dtp_dep_list.next,
				     struct tse_task_link, tl_link);
		d_list_del(&tlink->tl_link);
		task_tmp = tlink->tl_task;
		dtp_tmp = tse_task2priv(task_tmp);

		/* see if the dependent task is ready to be scheduled */
		D__ASSERT(dtp_tmp->dtp_dep_cnt > 0);
		dtp_tmp->dtp_dep_cnt--;
		D__DEBUG(DB_TRACE, "daos task %p dep_cnt %d\n", dtp_tmp,
			dtp_tmp->dtp_dep_cnt);
		if (!dsp->dsp_cancelling) {
			/*
			 * let's attach the current task to the dependent task,
			 * in case the dependent task needs to check the result
			 * of these tasks.
			 *
			 * NB: reuse tlink.
			 */
			tse_task_addref_locked(dtp);
			tlink->tl_task = task;
			d_list_add_tail(&tlink->tl_link,
					&dtp_tmp->dtp_ret_list);

			if (dtp_tmp->dtp_dep_cnt == 0 && dtp_tmp->dtp_running) {
				bool done;

				/*
				 * If the task is already running, let's mark it
				 * complete. This happens when we create
				 * subtasks in the body function of the main
				 * task. So the task function is done, but it
				 * will stay in the running state until all the
				 * tasks that it depends on are completed, then
				 * it is completed when they completed in this
				 * code block.
				 */

				dtp_tmp->dtp_completing = 1;
				/** release lock for CB */
				pthread_mutex_unlock(&dsp->dsp_lock);
				done = tse_task_complete_callback(task_tmp);
				pthread_mutex_lock(&dsp->dsp_lock);

				/*
				 * task reinserted itself in scheduler by
				 * calling tse_task_reinit().
				 */
				if (!done) {
					tse_task_decref_locked(dtp_tmp);
					continue;
				}

				tse_task_complete_locked(dtp_tmp, dsp);
			}
		} else {
			D__FREE_PTR(tlink);
		}

		/* -1 for tlink */
		tse_task_decref_locked(dtp_tmp);
	}

	D__ASSERT(dsp->dsp_inflight > 0);
	dsp->dsp_inflight--;
	pthread_mutex_unlock(&dsp->dsp_lock);

	if (task->dt_result == 0)
		task->dt_result = rc;

	return rc;
}

int
tse_sched_process_complete(struct tse_sched_private *dsp)
{
	struct tse_task_private *dtp;
	struct tse_task_private *tmp;
	d_list_t comp_list;
	int processed = 0;

	/* pick tasks from complete_list */
	D_INIT_LIST_HEAD(&comp_list);
	pthread_mutex_lock(&dsp->dsp_lock);
	d_list_splice_init(&dsp->dsp_complete_list, &comp_list);
	pthread_mutex_unlock(&dsp->dsp_lock);

	d_list_for_each_entry_safe(dtp, tmp, &comp_list, dtp_list) {
		tse_task_t *task = tse_priv2task(dtp);

		tse_task_post_process(task);
		d_list_del_init(&dtp->dtp_list);
		tse_task_decref(task);  /* drop final ref */
		processed++;
	}
	return processed;
}

bool
tse_sched_check_complete(tse_sched_t *sched)
{
	struct tse_sched_private *dsp = tse_sched2priv(sched);
	bool completed;

	/* check if all tasks are done */
	pthread_mutex_lock(&dsp->dsp_lock);
	completed = (d_list_empty(&dsp->dsp_init_list) &&
		     dsp->dsp_inflight == 0);
	pthread_mutex_unlock(&dsp->dsp_lock);

	return completed;
}

/* Run tasks for this schedule */
static void
tse_sched_run(tse_sched_t *sched)
{
	struct tse_sched_private *dsp = tse_sched2priv(sched);

	while (1) {
		int	processed = 0;
		bool	completed;

		processed += tse_sched_process_init(dsp);
		processed += tse_sched_process_complete(dsp);
		completed = tse_sched_check_complete(sched);
		if (completed || processed == 0)
			break;
	};

	/* drop reference of tse_sched_init() */
	tse_sched_decref(dsp);
}

/*
 * Poke the scheduler to run tasks in the init list if ready, finish tasks that
 * have completed.
 */
void
tse_sched_progress(tse_sched_t *sched)
{
	struct tse_sched_private *dsp = tse_sched2priv(sched);

	if (dsp->dsp_cancelling)
		return;

	pthread_mutex_lock(&dsp->dsp_lock);
	/** +1 for tse_sched_run() */
	tse_sched_addref_locked(dsp);
	pthread_mutex_unlock(&dsp->dsp_lock);

	if (!dsp->dsp_cancelling)
		tse_sched_run(sched);
	/** If another thread canceled, drop the ref count */
	else
		tse_sched_decref(dsp);
}

static int
tse_sched_complete_inflight(struct tse_sched_private *dsp)
{
	struct tse_task_private *dtp;
	struct tse_task_private *tmp;
	int			  processed = 0;

	pthread_mutex_lock(&dsp->dsp_lock);
	d_list_for_each_entry_safe(dtp, tmp, &dsp->dsp_running_list, dtp_list)
		if (dtp->dtp_dep_cnt == 0) {
			d_list_del(&dtp->dtp_list);
			tse_task_complete_locked(dtp, dsp);
			processed++;
		}
	pthread_mutex_unlock(&dsp->dsp_lock);

	return processed;
}

void
tse_sched_complete(tse_sched_t *sched, int ret, bool cancel)
{
	struct tse_sched_private *dsp = tse_sched2priv(sched);

	if (sched->ds_result == 0)
		sched->ds_result = ret;

	pthread_mutex_lock(&dsp->dsp_lock);
	if (dsp->dsp_cancelling || dsp->dsp_completing) {
		pthread_mutex_unlock(&dsp->dsp_lock);
		return;
	}

	if (cancel)
		dsp->dsp_cancelling = 1;
	else
		dsp->dsp_completing = 1;

	/** +1 for tse_sched_run */
	tse_sched_addref_locked(dsp);
	pthread_mutex_unlock(&dsp->dsp_lock);

	/** Wait for all in-flight tasks */
	while (1) {
		tse_sched_run(sched);
		if (dsp->dsp_inflight == 0)
			break;
		if (dsp->dsp_cancelling)
			tse_sched_complete_inflight(dsp);
	};

	tse_sched_complete_cb(sched);
	sched->ds_udata = NULL;
	tse_sched_decref(dsp);
}

void
tse_task_complete(tse_task_t *task, int ret)
{
	struct tse_task_private		*dtp	= tse_task2priv(task);
	struct tse_sched_private	*dsp	= dtp->dtp_sched;
	bool				bumped  = false;
	bool				done;

	if (dtp->dtp_completed)
		return;

	if (task->dt_result == 0)
		task->dt_result = ret;

	dtp->dtp_completing = 1;
	/** Execute task completion callbacks first. */
	done = tse_task_complete_callback(task);

	pthread_mutex_lock(&dsp->dsp_lock);

	if (!dsp->dsp_cancelling) {
		/** +1 for tse_sched_run() */
		tse_sched_addref_locked(dsp);
		/** track in case another thread cancels */
		bumped = true;

		/** if task reinserted itself in scheduler, don't complete */
		if (done)
			tse_task_complete_locked(dtp, dsp);
	} else {
		tse_task_decref_locked(dtp);
	}
	pthread_mutex_unlock(&dsp->dsp_lock);

	/** update task in scheduler lists. */
	if (!dsp->dsp_cancelling && done)
		tse_sched_process_complete(dsp);
	/** If another thread canceled, make sure we drop the ref count */
	else if (bumped)
		tse_sched_decref(dsp);

	/** -1 from tse_task_create() if it has not been reinitialized */
	if (done)
		tse_sched_decref(dsp);
}

/**
 * If one task dependents on other tasks, only if the dependent task
 * is done, then the task can be added to the scheduler list
 **/
static int
tse_task_add_dependent(tse_task_t *task, tse_task_t *dep)
{
	struct tse_task_private  *dtp = tse_task2priv(task);
	struct tse_task_private  *dep_dtp = tse_task2priv(dep);
	struct tse_task_link	  *tlink;

	if (dtp->dtp_sched != dep_dtp->dtp_sched) {
		D__ERROR("Two tasks should belong to the same scheduler.\n");
		return -DER_NO_PERM;
	}

	if (dtp->dtp_completed) {
		D__ERROR("Can't add a depedency for a completed task (%p)\n",
			task);
		return -DER_NO_PERM;
	}

	/** if task to depend on has completed already, do nothing */
	if (dep_dtp->dtp_completed)
		return 0;

	D__ALLOC_PTR(tlink);
	if (tlink == NULL)
		return -DER_NOMEM;

	D__DEBUG(DB_TRACE, "Add dependent %p ---> %p\n", dep_dtp, dtp);

	pthread_mutex_lock(&dtp->dtp_sched->dsp_lock);

	tse_task_addref_locked(dtp);
	tlink->tl_task = task;

	d_list_add_tail(&tlink->tl_link, &dep_dtp->dtp_dep_list);
	dtp->dtp_dep_cnt++;

	pthread_mutex_unlock(&dtp->dtp_sched->dsp_lock);

	return 0;
}

int
tse_task_register_deps(tse_task_t *task, int num_deps,
		       tse_task_t *dep_tasks[])
{
	int i;

	for (i = 0; i < num_deps; i++)
		tse_task_add_dependent(task, dep_tasks[i]);

	return 0;
}

int
tse_task_create(tse_task_func_t task_func, tse_sched_t *sched, void *priv,
		tse_task_t **taskp)
{
	struct tse_sched_private *dsp = tse_sched2priv(sched);
	struct tse_task_private	 *dtp;
	tse_task_t		 *task;

	D__ALLOC_PTR(task);
	if (task == NULL)
		return -DER_NOMEM;

	dtp = tse_task2priv(task);
	D_CASSERT(sizeof(task->dt_private) >= sizeof(*dtp));

	D_INIT_LIST_HEAD(&dtp->dtp_list);
	D_INIT_LIST_HEAD(&dtp->dtp_dep_list);
	D_INIT_LIST_HEAD(&dtp->dtp_comp_cb_list);
	D_INIT_LIST_HEAD(&dtp->dtp_prep_cb_list);
	D_INIT_LIST_HEAD(&dtp->dtp_ret_list);

	dtp->dtp_refcnt   = 1;
	dtp->dtp_func	  = task_func;
	dtp->dtp_priv	  = priv;
	dtp->dtp_sched	  = dsp;

	*taskp = task;
	return 0;
}

int
tse_task_schedule(tse_task_t *task, bool instant)
{
	struct tse_task_private  *dtp = tse_task2priv(task);
	struct tse_sched_private *dsp = dtp->dtp_sched;
	int rc = 0;

	D_ASSERT(!instant || dtp->dtp_func);

	/* Add task to scheduler */
	pthread_mutex_lock(&dsp->dsp_lock);
	if (dtp->dtp_func == NULL || instant) {
		/** If task has no body function, mark it as running */
		dsp->dsp_inflight++;
		dtp->dtp_running = 1;
		d_list_add_tail(&dtp->dtp_list, &dsp->dsp_running_list);

		/** +1 in case task is completed in body function */
		if (instant)
			tse_task_addref_locked(dtp);
	} else {
		/** Otherwise, scheduler will process it from init list */
		d_list_add_tail(&dtp->dtp_list, &dsp->dsp_init_list);
	}
	tse_sched_addref_locked(dsp);
	pthread_mutex_unlock(&dsp->dsp_lock);

	/* if caller wants to run the task instantly, call the task body
	 * function now.
	 */
	if (instant) {
		dtp->dtp_func(task);

		/** If task was completed return the task result */
		if (dtp->dtp_completed)
			rc = task->dt_result;

		tse_task_decref(task);
	}

	return rc;
}

int
tse_task_reinit(tse_task_t *task)
{
	struct tse_task_private		*dtp = tse_task2priv(task);
	tse_sched_t			*sched = tse_task2sched(task);
	struct tse_sched_private	*dsp = tse_sched2priv(sched);
	int				rc;

	D_CASSERT(sizeof(task->dt_private) >= sizeof(*dtp));

	pthread_mutex_lock(&dsp->dsp_lock);

	if (dsp->dsp_cancelling) {
		D__ERROR("Scheduler is cancelling, can't re-insert task\n");
		D__GOTO(err_unlock, rc = -DER_NO_PERM);
	}

	if (dtp->dtp_completed) {
		D__ERROR("Can't re-init a task that has completed already.\n");
		D__GOTO(err_unlock, rc = -DER_NO_PERM);
	}

	if (!dtp->dtp_running) {
		D__ERROR("Can't re-init a task that is not running.\n");
		D__GOTO(err_unlock, rc = -DER_NO_PERM);
	}

	if (dtp->dtp_func == NULL) {
		D__ERROR("Task body function can't be NULL.\n");
		D__GOTO(err_unlock, rc = -DER_INVAL);
	}

	/** Mark the task back at init state */
	dtp->dtp_running = 0;
	dtp->dtp_completing = 0;

	/** Task not in-flight anymore */
	dsp->dsp_inflight--;
	/** Move back to init list */
	d_list_move_tail(&dtp->dtp_list, &dsp->dsp_init_list);

	pthread_mutex_unlock(&dsp->dsp_lock);

	/** cleanup result task list */
	tse_task_ret_list_cleanup(task);

	return 0;

err_unlock:
	pthread_mutex_unlock(&dsp->dsp_lock);
	return rc;
}

int
tse_task_list_add(tse_task_t *task, d_list_t *head)
{
	struct tse_task_private *dtp = tse_task2priv(task);

	/* Note: this is export API, so once the task is scheduled,
	 * it is not allowed to be moved to any list by an outsider.
	 */
	if (dtp->dtp_running || dtp->dtp_completed ||
	    dtp->dtp_completing)
		return -DER_NO_PERM;

	D__ASSERT(d_list_empty(&dtp->dtp_list));
	d_list_add_tail(&dtp->dtp_list, head);
	return 0;
}

tse_task_t *
tse_task_list_first(d_list_t *head)
{
	struct tse_task_private	*dtp;

	if (d_list_empty(head))
		return NULL;

	dtp = d_list_entry(head->next, struct tse_task_private, dtp_list);
	return tse_priv2task(dtp);
}

void
tse_task_list_del(tse_task_t *task)
{
	struct tse_task_private *dtp = tse_task2priv(task);

	d_list_del_init(&dtp->dtp_list);
}

void
tse_task_list_sched(d_list_t *head, bool instant)
{
	while (!d_list_empty(head)) {
		tse_task_t *task = tse_task_list_first(head);

		tse_task_list_del(task);
		tse_task_schedule(task, instant);
	}
}

void
tse_task_list_abort(d_list_t *head, int rc)
{
	while (!d_list_empty(head)) {
		tse_task_t *task = tse_task_list_first(head);

		tse_task_list_del(task);
		tse_task_complete(task, rc);
	}
}

int
tse_task_list_depend(d_list_t *head, tse_task_t *task)
{
	struct tse_task_private *dtp;
	int			 rc;

	d_list_for_each_entry(dtp, head, dtp_list) {
		rc = tse_task_add_dependent(tse_priv2task(dtp), task);
		if (rc)
			return rc;
	}
	return 0;
}

int
tse_task_depend_list(tse_task_t *task, d_list_t *head)
{
	struct tse_task_private *dtp;
	int			 rc;

	d_list_for_each_entry(dtp, head, dtp_list) {
		rc = tse_task_add_dependent(task, tse_priv2task(dtp));
		if (rc)
			return rc;
	}
	return 0;
}
