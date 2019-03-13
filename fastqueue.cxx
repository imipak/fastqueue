#include <list>
#include <mutex>
#include <condition_variable>
#include <stdio.h>

using namespace std;

#if !defined(size_type)
typedef size_t size_type;
#endif

// Template prototype for manupulating a queue of arbitrary blobs
// of data
  
template <class DataType> class FastQueue
{
  private:
    mutex mtx;
    condition_variable condition;
    size_type count; // We are using a counter because certain C++ libraries seem to return incorrect sizes

  public:
    list<DataType> blobQueue;

    // Function for clearing the queue
    void     init()
    		{
	          count = 0;
		  blobQueue.clear();
		}

    // Blocking POP operation 
    DataType pop()
    		{
		  // Set up a semaphore using mutexes to signal when blocked
  		  if (count == 0)
		  {
		     unique_lock<mutex> lock(mtx);
		     condition.wait(lock);
		  }

		  // Always lock critical portions of multithreaded code
		  mtx.lock();
		  DataType dataBlob = blobQueue.front();
		  blobQueue.pop_front();
		  count--;
		  mtx.unlock();
		  return(dataBlob);
    		}

    // Non-blocking POP operation, return a nullptr on error
    DataType pop_try()
    		{
		  DataType dataBlob;

		  // We lock the whole function here for safety
		  mtx.lock();
		  if (count == 0)
		  {
		    dataBlob = nullptr;
		  }
		  else
		  {
		    dataBlob = blobQueue.front();
		    blobQueue.pop_front();
		    count--;
		  }
		  mtx.unlock();
		  return(dataBlob);
    		}

    // PUSH operation doesn't need a lock since there's no race condition
    void     push(DataType dataBlob)
    		{
		  blobQueue.push_back(dataBlob);
		  count++;
		  if (blobQueue.size() != 0)
		  {
		    // On successful push operation, signal the next POP operation that they can proceed
		    condition.notify_one();
		  }
		  return;
    		}
};

