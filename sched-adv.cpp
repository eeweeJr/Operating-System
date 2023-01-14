/*
 * The Priority Task Scheduler
 * SKELETON IMPLEMENTATION TO BE FILLED IN FOR TASK 1
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
class AdvancedSchedulingAlgorithm : public SchedulingAlgorithm
{
public:
    /**
     * Returns the friendly name of the algorithm, for debugging and selection purposes.
     */
    const char* name() const override { return "adv"; }

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
     * Called to determine if an object is in a queue and not runnable.
     * @param queue : Queue of scheduling entities
     */ 

    bool stopped(List<SchedulingEntity *> queue) 
    {
        if (!queue.empty()) {
            while (!inQueue(queue.first(),runqueue)) {
                    return true;
                    if (highPriority.empty()) {
                            break;
                        }
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
        if (runqueue.count() == 0) {
            return NULL;
        }
		if (runqueue.count() == 1) {
            return runqueue.first();
        }
    
        SchedulingEntity *RunningEntity = NULL;

        //Sorting Scheduling entities of runqueue into seperate queues based off of priority level, Low priority or High priority
        for (const auto& entity : runqueue) {
            if (entity->priority() == 0) {
                if (!inQueue(entity, highPriority)){
                    highPriority.push(entity);
                    }
			  }
            else if (entity->priority() == 1) {
                if (!inQueue(entity, highPriority)){
                    highPriority.append(entity);
                    }
		        }
             else if (entity->priority() == 2) {
                  if (!inQueue(entity, lowPriority)) { 
                      lowPriority.append(entity);
                      }
             }

            else if (entity->priority() == 3){
                if (!inQueue(entity, lowPriority)){
                    lowPriority.append(entity);
                    }
                }
            else if (entity->priority() > 3 || entity->priority()< 0){
                syslog.messagef(LogLevel::ERROR, "Entity is not of any known priority level");
                }
            }

        if ((!lowPriority.empty() && stopped(highPriority)) ||highPriority.empty() || (!stopped(lowPriority) && lowPriority.count()==1)) {
            if (!lowPriority.empty()) {
                while (!inQueue(lowPriority.first(),runqueue)) {
                    lowPriority.remove(lowPriority.first());
                        if (lowPriority.empty()) {
                        break;
                    }
                }
            }
            if (!highPriority.empty()) {
                while (!inQueue(highPriority.first(),runqueue)) {
                    highPriority.remove(highPriority.first());
                        if (highPriority.empty()) {
                        break;
                    }
                }
            }
            RunningEntity = lowPriority.first();
            lowPriority.enqueue(lowPriority.dequeue());
            return RunningEntity;
        }


        if (!highPriority.empty()) {
            if (!highPriority.empty()) {
                RunningEntity = highPriority.first();
                highPriority.enqueue(highPriority.dequeue());
                return RunningEntity;
            }
        }
        syslog.messagef(LogLevel::ERROR,"Queue Empty");
        return NULL;
        }
    private: List<SchedulingEntity *> runqueue;
    private: List<SchedulingEntity *> highPriority;
    private: List<SchedulingEntity *> lowPriority;

};

/* --- DO NOT CHANGE ANYTHING BELOW THIS LINE --- */

RegisterScheduler(AdvancedSchedulingAlgorithm);