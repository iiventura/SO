#include "sched.h"

struct prior_data {
	int remaining_ticks_slice;
};

static task_t* pick_next_task_prior(runqueue_t* rq)
{
	task_t* next_task=head_slist(&rq->tasks);
	if (next_task) {
	    remove_slist(&rq->tasks,next_task);
	    next_task->on_rq=FALSE;
	    rq->cur_task=next_task;
	 }

	return next_task;
}

static int compare_priority(void *t1,void *t2)
{
	task_t* tsk1=(task_t*)t1;
	task_t* tsk2=(task_t*)t2;
	return tsk1->prio-tsk2->prio;
}


static int task_new_prior(task_t* t)
{
	struct prior_data* data=malloc(sizeof(struct prior_data));
	if (!data) return 1;
	t->tcs_data=data;
	return 0;
}

static void task_free_prior(task_t* t)
{
	if (t->tcs_data) {
		free(t->tcs_data);
		t->tcs_data=NULL;
	}
}

static void enqueue_task_prior(task_t* t,runqueue_t* rq, int runnable)
{
	if (t->on_rq || is_idle_task(t))return;

	if (t->flags & TF_INSERT_FRONT) {
		t->flags&=~TF_INSERT_FRONT;
		sorted_insert_slist_front(&rq->tasks, t, 1, compare_priority);
	} else
		sorted_insert_slist(&rq->tasks, t, 1, compare_priority);


	if (!runnable) {
		task_t* current=rq->cur_task;
		if (preemptive_scheduler && t->prio<current->prio) {
			rq->need_resched=TRUE;
			current->flags|=TF_INSERT_FRONT;
		}
	}
}

static task_t* steal_task_prior(runqueue_t* rq)
{
	task_t* steal_task=tail_slist(&rq->tasks);
	if (steal_task) {
	   remove_slist(&rq->tasks,steal_task);
	   steal_task->on_rq=FALSE;
	   rq->nr_runnable--;
	}
	return steal_task;
}


static void task_tick_prior(runqueue_t* rq)
{
	task_t* current_task=rq->cur_task;
	struct prior_data* data=(struct prior_data*) current_task->tcs_data;
	if (is_idle_task(current_task))return;
	data->remaining_ticks_slice--; /* Charge tick */
	if (data->remaining_ticks_slice<=0) rq->need_resched=TRUE;
}



sched_class_t prio_sched ={
	.pick_next_task=pick_next_task_prior,
	.task_new=task_new_prior,
	.task_free=task_free_prior,
	.enqueue_task=enqueue_task_prior,
	.task_tick=task_tick_prior,
	.steal_task=steal_task_prior
};
