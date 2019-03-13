#include <vector>
#include <random>
#include <thread>
#include <stdint.h>
#include <stdio.h>
#include "fastqueue.cxx"

typedef  vector<uint32_t>  BlobType;
typedef  BlobType         *BlobPtr;

static   FastQueue<BlobPtr> fast;

void reader(uint32_t tmax)
{
  size_type totalCapacity = 1024 * tmax; 
  time_t    start;
  time_t    end;
  BlobPtr   localObjectPtr;
  uint32_t  pos;
  double    diff;

  random_device dev;
  mt19937 rng(dev());
  uniform_int_distribution<std::mt19937::result_type> dist(0, 1);

  time(&start);
  pos = 0;
  while (pos < totalCapacity)
  {
    if (dist(rng) == 0)
    {
      localObjectPtr = fast.pop();
    }
    else
    {
      localObjectPtr = fast.pop_try();
    }
    if (localObjectPtr == nullptr)
    {
      printf("Queue empty\n");
    }
      else
    {
      printf("Queue %u read\n", pos);
      pos++;
    }
    printf("Processed %u entries.\n", pos);
  }
  time(&end);
  diff = difftime(end, start);
  printf("%.f seconds to read back all objects\n", diff);
}

void writer(uint32_t tnum)
{
  size_type queueSize = 1024;
  BlobType  localObjectList;
  time_t    start;
  time_t    end;
  double    diff;
  uint32_t  pos;
  uint32_t  identity = tnum;

  random_device dev;
  mt19937 rng(dev());
  uniform_int_distribution<std::mt19937::result_type> dist(0, 32767);

  for (pos = 0; pos < queueSize; pos++)
  {
    printf("Generating item %ul\n", pos);
    localObjectList.push_back(dist(rng));
  }
  time(&start);
  for (pos = 0; pos < queueSize; pos++)
  {
    printf("Pushing item %ul\n", pos);
    fast.push(&localObjectList);
  }
  time(&end);
  diff = difftime(end, start);
  printf("%.f seconds to write %lu objects on thread %u.\n", diff, queueSize, identity);
}

int main()
{
  uint32_t queueSize = 256;
  uint32_t threadLim = 2;
  uint32_t threadNum;
  uint32_t totalSize;
  thread   writeThread[threadLim];

  BlobType arrayOfObjects[queueSize];

  fast.init();

  for (threadNum = 0; threadNum < threadLim; threadNum++)
  {
    writeThread[threadNum] = thread(writer, 0);
    writeThread[threadNum].join();
  }
  totalSize = fast.blobQueue.size();
  printf("Total number of items is: %u.\n", totalSize);
  thread readThread = thread(reader, 1);
  readThread.join();
  return(0);
}

