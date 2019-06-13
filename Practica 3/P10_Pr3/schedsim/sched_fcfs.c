#include "sched.h"

static void enqueue_task_fcfs(task_t* t,runqueue_t* rq, int preempted)
{
	if (t->on_rq || is_idle_task(t)) return;
	insert_slist(&rq->tasks,t);
	t->on_rq=TRUE;

	if (!preempted){
		rq->nr_runnable++;
	}
}

static task_t* steal_task_fcfs(runqueue_t* rq)
{
	task_t* steal_task=tail_slist(&rq->tasks);
	if (steal_task) {
		remove_slist(&rq->tasks,steal_task);
		steal_task->on_rq=FALSE;
	    rq->nr_runnable--;
	 }
	return steal_task;
}

static void task_tick_fcfs(runqueue_t* rq){

    task_t* current_task=rq->cur_task;

    if (is_idle_task(current_task))
        return;

    if (current_task->runnable_ticks_left==1)
        rq->nr_runnable--;
}

static task_t* pick_next_task_fcfs(runqueue_t* rq)
{
	task_t* next_task=head_slist(&rq->tasks);
	if (next_task) {
		remove_slist(&rq->tasks,next_task);
		next_task->on_rq=FALSE;
		rq->cur_task=next_task;

	}
	return next_task;
}


sched_class_t fcfs_sched ={
	.pick_next_task=pick_next_task_fcfs,
	.enqueue_task=enqueue_task_fcfs,
	.steal_task=steal_task_fcfs,
	.task_tick=task_tick_fcfs,
};
