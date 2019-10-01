#ifndef ReadingsQueue_h
#define ReadingsQueue_h

#define MAX_SIZE 10

struct Reading
{
  int id_sensor;
  float reading;
  char timestamp[17];
};

class ReadingsQueue
{
    public:
        Reading pop();
        int size();

        bool add(Reading*);
        bool is_empty();
        bool is_full();

    private:
        Reading _queue[MAX_SIZE];
        int _front = 0;
        int _rear = 0;
        int _size = 0;
};

#endif