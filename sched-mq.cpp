/*
 * The Priority Task Scheduler
 */

#include <infos/kernel/sched.h>
#include <infos/kernel/thread.h>
#include <infos/kernel/log.h>
#include <infos/util/list.h>
#include <infos/util/lock.h>

using namespace infos::kernel;
using namespace infos::util;

/**
 * A Multiple Queue priority scheduling algorithm
 */
class MultipleQueuePriorityScheduler : public SchedulingAlgorithm
{
public:
    /**
     * Returns the friendly name of the algorithm, for debugging and selection purposes.
     */
    const char* name() const override { return "mq"; }

    /**
     * Called during scheduler initialisation.
     */
    void init()
    {
    }

    /**
     * Called when a scheduling entity becomes eligible for running.
     * @param entity
     */
    void add_to_runqueue(SchedulingEntity& entity) override
    {
		UniqueIRQLock l;
		runqueue.enqueue(&entity);
    }
    /**
     * Called when a scheduling entity is no longer eligible for running.
     * @param entity
     */
    void remove_from_runqueue(SchedulingEntity& entity) override
    {
		UniqueIRQLock l;
		runqueue.remove(&entity);
    }

    /**
     * determines if a scheduling entity is present within a queue.
     * @param queue: queue where entity is checked for within
     * @param entity: entity we are checking is contained within the queue
     */

    bool inQueue(SchedulingEntity* entity,  List<SchedulingEntity *> queue)
    {
      for (const auto& entityInQueue : queue) {
        if (entity == entityInQueue) {
          return true;
        }
      }
      return false;
    }


    /**
     * Called every time a scheduling event occurs, to cause the next eligible entity
     * to be chosen.  The next eligible entity might actually be the same entity, if
     * e.g. its timeslice has not expired.
     */
    SchedulingEntity *pick_next_entity() override {

		if (runqueue.count() == 0) return NULL;
		if (runqueue.count() == 1) return runqueue.first();
    
    SchedulingEntity *RunningEntity = NULL;

    //bools to indicate the existance of higher priorty entities
    bool exists0 = true;
    bool exists1 = true;
    bool exists2 = true;


    //Sorting Scheduling entities of runqueue into seperate queues based off of priority level
    for (const auto& entity : runqueue) {
      if (entity->priority() == 0) {
        if (!inQueue(entity, queue0)){
          exists0 = false;
          queue0.enqueue(entity);
          }
			  }
      else if (entity->priority() == 1 && exists0) {
        if (!inQueue(entity, queue1)){
          exists1 = false;
          queue1.enqueue(entity);
          }
		    }
      else if (entity->priority() == 2 && (exists0 || exists1)) {
        if (!inQueue(entity, queue2)) {
          exists2 = false; 
          queue2.enqueue(entity);
          }
        }
      else if (entity->priority() == 3 && (exists0 || exists1 || exists2)){
        if (!inQueue(entity, queue3)){
          queue3.enqueue(entity);
        }
      }
      else if (entity->priority() > 3 || entity->priority()< 0){
      syslog.messagef(LogLevel::ERROR, "Entity is not of any known priority level");
      }
    }




  if (!queue0.empty()) {
    while (!inQueue(queue0.first(),runqueue)) {
      queue0.remove(queue0.first());
      if (queue0.empty()) {
          break;
        } 
      }
      if (!queue0.empty()) {
          RunningEntity = queue0.first();
          queue0.enqueue(queue0.dequeue());
          return RunningEntity;
      }
    }

    
    if (!queue1.empty()) {
      while (!inQueue(queue1.first(),runqueue)) {
        queue1.remove(queue1.first());
        if (queue1.empty()) {
          break;
        } 
      }
      if (!queue1.empty()) {
          RunningEntity = queue1.first();
          queue1.enqueue(queue1.dequeue());
          return RunningEntity;
      }
    }


    if (!queue2.empty()) {     
      while (!inQueue(queue2.first(),runqueue)) {
        queue2.remove(queue2.first());
        if (queue2.empty()) {
          break;
        } 
      }
      if (!queue2.empty()) {
          RunningEntity = queue2.first();
          queue2.enqueue(queue2.dequeue());
          return RunningEntity;
      }
    }


    if (!queue3.empty()) { 
      while (!inQueue(queue3.first(),runqueue)) {
        queue3.remove(queue3.first());
        if (queue3.empty()) {
          break;
        } 
      }
      if (!queue3.empty()) {
          RunningEntity = queue3.first();
          queue3.enqueue(queue3.dequeue());
          return RunningEntity;
      }
    }
    else {
      syslog.messagef(LogLevel::ERROR, "Entity is not of any known priority level");
      return NULL;
    }
    return NULL;
    }
    private: List<SchedulingEntity *> runqueue;

    //queues signifying prioirity levels 0 to 3.
    private: List<SchedulingEntity *> queue0;
    private: List<SchedulingEntity *> queue1;
    private: List<SchedulingEntity *> queue2;
    private: List<SchedulingEntity *> queue3;

};


RegisterScheduler(MultipleQueuePriorityScheduler);
