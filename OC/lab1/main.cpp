#include <iostream>
#include <string>
#include <pthread.h>
#include <unistd.h>

class Event {
public:
    int id;
    std::string payload;

    Event(int eventId, const std::string& data)
        : id(eventId), payload(data) {}
};

class Monitor {
private:
    pthread_mutex_t mutex;
    pthread_cond_t condition;
    bool ready;
    bool stopping;
    Event* data;

public:
    Monitor() : ready(false), stopping(false), data(nullptr) {
        pthread_mutex_init(&mutex, nullptr);
        pthread_cond_init(&condition, nullptr);
    }

    ~Monitor() {
        pthread_cond_destroy(&condition);
        pthread_mutex_destroy(&mutex);
    }

    bool provide(int eventId) {
        pthread_mutex_lock(&mutex);

        if (ready) {
            pthread_mutex_unlock(&mutex);
            return false;
        }

        data = new Event(eventId, "объект");
        ready = true;

        std::cout << "Поставщик: отправлено событие #" << data->id 
                  << " (адрес объекта: " << data << ")" << std::endl;

        pthread_cond_signal(&condition);
        pthread_mutex_unlock(&mutex);
        return true;
    }

    bool consume() {
        pthread_mutex_lock(&mutex);

        while (!ready && !stopping) {
            pthread_cond_wait(&condition, &mutex);
        }

        if (stopping && !ready) {
            pthread_mutex_unlock(&mutex);
            return false;
        }

        Event* event = data;
        data = nullptr;
        ready = false;

        std::cout << "Потребитель: получено событие #" << event->id 
                  << " (адрес объекта: " << event << ")" << std::endl;

        delete event;

        pthread_mutex_unlock(&mutex);
        return true;
    }

    void stop() {
        pthread_mutex_lock(&mutex);
        stopping = true;
        pthread_cond_broadcast(&condition);
        pthread_mutex_unlock(&mutex);
    }
};

void* producerThread(void* arg) {
    Monitor* monitor = static_cast<Monitor*>(arg);
    const int totalEvents = 5;

    for (int i = 1; i <= totalEvents; ) {
        sleep(1);
        if (monitor->provide(i)) {
            ++i;
        }
    }

    monitor->stop();
    return nullptr;
}

void* consumerThread(void* arg) {
    Monitor* monitor = static_cast<Monitor*>(arg);

    while (monitor->consume()) {
    }

    return nullptr;
}

int main() {
    Monitor monitor;
    pthread_t producer, consumer;

    pthread_create(&consumer, nullptr, consumerThread, &monitor);
    pthread_create(&producer, nullptr, producerThread, &monitor);

    pthread_join(producer, nullptr);
    pthread_join(consumer, nullptr);

    std::cout << "Готово." << std::endl;
    return 0;
}
