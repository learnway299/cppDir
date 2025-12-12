# Qt 多线程（Multithreading）

## 概述

Qt 提供了完整的多线程支持，包括底层的 QThread、高级的 Qt Concurrent，以及线程安全的信号槽机制。理解 Qt 多线程对于开发高性能、响应式的应用程序至关重要。

---

## 1. QThread 基础

### 1.1 QThread 的两种使用方式

**面试问题**：QThread 有哪两种主要使用方式？推荐哪种？

**答案**：

```cpp
// 方式 1：继承 QThread，重写 run()（不推荐）
class WorkerThread : public QThread {
    Q_OBJECT

protected:
    void run() override {
        qDebug() << "Thread running:" << QThread::currentThreadId();

        // 执行耗时操作
        for (int i = 0; i < 100; ++i) {
            msleep(100);
            emit progressChanged(i);
        }

        emit finished();
    }

signals:
    void progressChanged(int value);
    void finished();
};

// 使用
WorkerThread* thread = new WorkerThread();
connect(thread, &WorkerThread::progressChanged, this, &Widget::updateProgress);
connect(thread, &WorkerThread::finished, thread, &QObject::deleteLater);
thread->start();


// 方式 2：moveToThread（推荐）
class Worker : public QObject {
    Q_OBJECT

public slots:
    void doWork() {
        qDebug() << "Working in thread:" << QThread::currentThreadId();

        for (int i = 0; i < 100; ++i) {
            QThread::msleep(100);
            emit progressChanged(i);
        }

        emit finished();
    }

signals:
    void progressChanged(int value);
    void finished();
};

// 使用
QThread* thread = new QThread();
Worker* worker = new Worker();

worker->moveToThread(thread);

connect(thread, &QThread::started, worker, &Worker::doWork);
connect(worker, &Worker::finished, thread, &QThread::quit);
connect(worker, &Worker::finished, worker, &QObject::deleteLater);
connect(thread, &QThread::finished, thread, &QObject::deleteLater);

thread->start();
```

**为什么推荐 moveToThread**：

| 方面 | 继承 QThread | moveToThread |
|------|-------------|--------------|
| 职责分离 | 混合线程管理和业务逻辑 | 清晰分离 |
| 事件循环 | 需手动调用 exec() | 自动运行 |
| 信号槽 | 容易出错 | 正常工作 |
| 定时器 | 需特殊处理 | 正常工作 |
| 可测试性 | 差 | 好 |

---

### 1.2 线程生命周期管理

**面试问题**：如何正确管理 QThread 的生命周期？

**答案**：

```cpp
class ThreadManager : public QObject {
    Q_OBJECT

public:
    void startWork() {
        m_thread = new QThread(this);
        m_worker = new Worker();  // 不设置父对象！

        m_worker->moveToThread(m_thread);

        // 正确的连接顺序
        connect(m_thread, &QThread::started,
                m_worker, &Worker::doWork);

        connect(m_worker, &Worker::finished,
                m_thread, &QThread::quit);

        connect(m_worker, &Worker::finished,
                m_worker, &QObject::deleteLater);

        connect(m_thread, &QThread::finished,
                m_thread, &QObject::deleteLater);

        m_thread->start();
    }

    void stopWork() {
        if (m_thread && m_thread->isRunning()) {
            // 请求中断
            m_worker->requestInterrupt();

            // 等待线程结束（带超时）
            if (!m_thread->wait(5000)) {
                qWarning() << "Thread did not finish in time, terminating...";
                m_thread->terminate();  // 危险：最后手段
                m_thread->wait();
            }
        }
    }

    ~ThreadManager() {
        stopWork();
    }

private:
    QThread* m_thread = nullptr;
    Worker* m_worker = nullptr;
};

// Worker 支持中断
class Worker : public QObject {
    Q_OBJECT

public:
    void requestInterrupt() {
        m_interrupted.storeRelease(true);
    }

public slots:
    void doWork() {
        for (int i = 0; i < 1000; ++i) {
            // 检查中断标志
            if (m_interrupted.loadAcquire()) {
                qDebug() << "Work interrupted";
                break;
            }

            processItem(i);
        }

        emit finished();
    }

signals:
    void finished();

private:
    QAtomicInt m_interrupted{false};
};
```

---

### 1.3 线程优先级

**面试问题**：Qt 如何设置线程优先级？

**答案**：

```cpp
void threadPriorityDemo() {
    QThread* thread = new QThread();

    // 启动前设置优先级
    thread->start(QThread::HighPriority);

    // 或者启动后设置
    thread->setPriority(QThread::LowPriority);

    // 优先级枚举：
    // QThread::IdlePriority        - 最低，仅在空闲时运行
    // QThread::LowestPriority      - 低于 LowPriority
    // QThread::LowPriority         - 低于 NormalPriority
    // QThread::NormalPriority      - 默认优先级
    // QThread::HighPriority        - 高于 NormalPriority
    // QThread::HighestPriority     - 高于 HighPriority
    // QThread::TimeCriticalPriority - 最高，尽可能频繁调度
    // QThread::InheritPriority     - 继承创建线程的优先级

    // 获取当前优先级
    QThread::Priority priority = thread->priority();
}
```

---

## 2. 线程同步

### 2.1 QMutex

**面试问题**：如何使用 QMutex 保护共享数据？

**答案**：

```cpp
#include <QMutex>
#include <QMutexLocker>

class ThreadSafeCounter {
public:
    void increment() {
        // 方式 1：手动加锁解锁
        m_mutex.lock();
        ++m_value;
        m_mutex.unlock();
    }

    void decrement() {
        // 方式 2：使用 QMutexLocker（RAII，推荐）
        QMutexLocker locker(&m_mutex);
        --m_value;
        // 自动解锁
    }

    int value() const {
        QMutexLocker locker(&m_mutex);
        return m_value;
    }

private:
    mutable QMutex m_mutex;
    int m_value = 0;
};

// 递归锁
class RecursiveExample {
public:
    void methodA() {
        QMutexLocker locker(&m_mutex);
        // 可以调用 methodB，因为是递归锁
        methodB();
    }

    void methodB() {
        QMutexLocker locker(&m_mutex);  // 同一线程可再次获取
        // ...
    }

private:
    QRecursiveMutex m_mutex;  // Qt 5.14+ 推荐用这个
    // 或 QMutex m_mutex{QMutex::Recursive}; // Qt 5.x
};
```

---

### 2.2 QReadWriteLock

**面试问题**：什么时候使用读写锁？

**答案**：

```cpp
#include <QReadWriteLock>
#include <QReadLocker>
#include <QWriteLocker>

class ThreadSafeCache {
public:
    QString get(const QString& key) const {
        // 读锁：多个线程可同时读
        QReadLocker locker(&m_lock);
        return m_cache.value(key);
    }

    void set(const QString& key, const QString& value) {
        // 写锁：独占访问
        QWriteLocker locker(&m_lock);
        m_cache[key] = value;
    }

    bool contains(const QString& key) const {
        QReadLocker locker(&m_lock);
        return m_cache.contains(key);
    }

    void remove(const QString& key) {
        QWriteLocker locker(&m_lock);
        m_cache.remove(key);
    }

private:
    mutable QReadWriteLock m_lock;
    QHash<QString, QString> m_cache;
};

// 使用场景：读多写少
// 读写比例建议：读操作 > 10 * 写操作 才有明显收益
```

---

### 2.3 QWaitCondition

**面试问题**：如何实现生产者-消费者模式？

**答案**：

```cpp
#include <QWaitCondition>
#include <QQueue>

template<typename T>
class BlockingQueue {
public:
    explicit BlockingQueue(int maxSize = 100)
        : m_maxSize(maxSize) {}

    void enqueue(const T& item) {
        QMutexLocker locker(&m_mutex);

        // 等待队列有空间
        while (m_queue.size() >= m_maxSize) {
            m_notFull.wait(&m_mutex);
        }

        m_queue.enqueue(item);
        m_notEmpty.wakeOne();  // 唤醒一个等待的消费者
    }

    T dequeue() {
        QMutexLocker locker(&m_mutex);

        // 等待队列非空
        while (m_queue.isEmpty()) {
            m_notEmpty.wait(&m_mutex);
        }

        T item = m_queue.dequeue();
        m_notFull.wakeOne();  // 唤醒一个等待的生产者
        return item;
    }

    bool tryDequeue(T& item, int timeout = 0) {
        QMutexLocker locker(&m_mutex);

        if (m_queue.isEmpty()) {
            if (timeout <= 0) return false;
            if (!m_notEmpty.wait(&m_mutex, timeout)) {
                return false;  // 超时
            }
        }

        if (m_queue.isEmpty()) return false;

        item = m_queue.dequeue();
        m_notFull.wakeOne();
        return true;
    }

    int size() const {
        QMutexLocker locker(&m_mutex);
        return m_queue.size();
    }

private:
    mutable QMutex m_mutex;
    QWaitCondition m_notEmpty;
    QWaitCondition m_notFull;
    QQueue<T> m_queue;
    int m_maxSize;
};

// 生产者
class Producer : public QThread {
protected:
    void run() override {
        for (int i = 0; i < 100; ++i) {
            m_queue->enqueue(i);
            msleep(10);
        }
    }

public:
    BlockingQueue<int>* m_queue;
};

// 消费者
class Consumer : public QThread {
protected:
    void run() override {
        while (!isInterruptionRequested()) {
            int item;
            if (m_queue->tryDequeue(item, 100)) {
                process(item);
            }
        }
    }

public:
    BlockingQueue<int>* m_queue;
};
```

---

### 2.4 QSemaphore

**面试问题**：信号量有什么用途？

**答案**：

```cpp
#include <QSemaphore>

// 用途 1：限制并发数量
class ConnectionPool {
public:
    explicit ConnectionPool(int maxConnections)
        : m_semaphore(maxConnections) {}

    void acquireConnection() {
        m_semaphore.acquire();  // 获取一个许可
        // 现在可以使用连接
    }

    void releaseConnection() {
        m_semaphore.release();  // 释放许可
    }

    bool tryAcquire(int timeout = 0) {
        return m_semaphore.tryAcquire(1, timeout);
    }

private:
    QSemaphore m_semaphore;
};

// 用途 2：环形缓冲区
class RingBuffer {
public:
    static const int BufferSize = 1024;

    RingBuffer()
        : m_freeSpace(BufferSize)
        , m_usedSpace(0) {}

    void write(const char* data, int size) {
        m_freeSpace.acquire(size);

        // 写入数据到缓冲区
        // ...

        m_usedSpace.release(size);
    }

    int read(char* buffer, int maxSize) {
        int available = qMin(maxSize, m_usedSpace.available());
        m_usedSpace.acquire(available);

        // 从缓冲区读取数据
        // ...

        m_freeSpace.release(available);
        return available;
    }

private:
    QSemaphore m_freeSpace;
    QSemaphore m_usedSpace;
    char m_buffer[BufferSize];
};
```

---

## 3. Qt Concurrent

### 3.1 QtConcurrent::run

**面试问题**：如何使用 QtConcurrent 执行并行任务？

**答案**：

```cpp
#include <QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>

class ConcurrentExample : public QObject {
    Q_OBJECT

public:
    // 简单的异步执行
    void simpleAsync() {
        QFuture<void> future = QtConcurrent::run([]() {
            qDebug() << "Running in thread:" << QThread::currentThreadId();
            QThread::sleep(2);
        });

        // 等待完成（阻塞）
        future.waitForFinished();
    }

    // 带返回值
    void asyncWithResult() {
        QFuture<int> future = QtConcurrent::run([]() {
            int sum = 0;
            for (int i = 0; i < 1000000; ++i) {
                sum += i;
            }
            return sum;
        });

        // 使用 watcher 监控
        QFutureWatcher<int>* watcher = new QFutureWatcher<int>(this);

        connect(watcher, &QFutureWatcher<int>::finished, this, [this, watcher]() {
            int result = watcher->result();
            qDebug() << "Result:" << result;
            watcher->deleteLater();
        });

        watcher->setFuture(future);
    }

    // 带参数
    void asyncWithParams(const QString& filename) {
        QFuture<QByteArray> future = QtConcurrent::run([filename]() {
            QFile file(filename);
            if (file.open(QIODevice::ReadOnly)) {
                return file.readAll();
            }
            return QByteArray();
        });

        // ...
    }
};
```

---

### 3.2 并行算法

**面试问题**：Qt Concurrent 提供了哪些并行算法？

**答案**：

```cpp
#include <QtConcurrent>

void parallelAlgorithms() {
    QList<int> list = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // 1. map - 原地变换
    QtConcurrent::map(list, [](int& value) {
        value *= 2;
    }).waitForFinished();

    // 2. mapped - 返回新容器
    QList<QString> strings = QtConcurrent::mapped(list, [](int value) {
        return QString::number(value);
    }).results();

    // 3. filter - 原地过滤
    QtConcurrent::filter(list, [](int value) {
        return value % 2 == 0;  // 保留偶数
    }).waitForFinished();

    // 4. filtered - 返回新容器
    QList<int> evens = QtConcurrent::filtered(list, [](int value) {
        return value % 2 == 0;
    }).results();

    // 5. reduce - 归约
    int sum = QtConcurrent::mappedReduced(
        list,
        [](int value) { return value; },           // map 函数
        [](int& result, int value) { result += value; }, // reduce 函数
        QtConcurrent::SequentialReduce            // 顺序 reduce
    ).result();

    // 6. filteredReduced
    int evenSum = QtConcurrent::filteredReduced(
        list,
        [](int value) { return value % 2 == 0; }, // filter
        [](int& result, int value) { result += value; } // reduce
    ).result();
}

// 图像处理示例
void processImages(const QStringList& imagePaths) {
    // 并行处理多个图像
    QFuture<QImage> future = QtConcurrent::mapped(imagePaths, [](const QString& path) {
        QImage image(path);
        // 处理图像...
        return image.scaled(100, 100, Qt::KeepAspectRatio);
    });

    QFutureWatcher<QImage>* watcher = new QFutureWatcher<QImage>();

    connect(watcher, &QFutureWatcher<QImage>::resultReadyAt,
            [watcher](int index) {
        QImage result = watcher->resultAt(index);
        qDebug() << "Image" << index << "processed";
    });

    connect(watcher, &QFutureWatcher<QImage>::finished,
            [watcher]() {
        QList<QImage> results = watcher->future().results();
        qDebug() << "All" << results.size() << "images processed";
        watcher->deleteLater();
    });

    watcher->setFuture(future);
}
```

---

### 3.3 QFuture 和 QPromise (Qt 6)

**面试问题**：Qt 6 的 QPromise 是什么？

**答案**：

```cpp
#include <QPromise>
#include <QFuture>

// Qt 6 的 QPromise 允许手动控制 future
class AsyncTaskManager : public QObject {
    Q_OBJECT

public:
    QFuture<int> startAsyncTask() {
        QPromise<int> promise;
        QFuture<int> future = promise.future();

        // 在线程中执行
        QtConcurrent::run([promise = std::move(promise)]() mutable {
            promise.start();

            for (int i = 0; i <= 100; ++i) {
                if (promise.isCanceled()) {
                    promise.finish();
                    return;
                }

                // 报告进度
                promise.setProgressRange(0, 100);
                promise.setProgressValue(i);

                // 可以添加中间结果
                if (i % 10 == 0) {
                    promise.addResult(i);
                }

                QThread::msleep(50);
            }

            promise.finish();
        });

        return future;
    }
};

// 使用
void usePromise() {
    AsyncTaskManager manager;
    QFuture<int> future = manager.startAsyncTask();

    QFutureWatcher<int>* watcher = new QFutureWatcher<int>();

    QObject::connect(watcher, &QFutureWatcher<int>::progressValueChanged,
                     [](int progress) {
        qDebug() << "Progress:" << progress << "%";
    });

    QObject::connect(watcher, &QFutureWatcher<int>::resultReadyAt,
                     [watcher](int index) {
        qDebug() << "Result at" << index << ":" << watcher->resultAt(index);
    });

    watcher->setFuture(future);

    // 取消任务
    // future.cancel();
}
```

---

## 4. 线程池

### 4.1 QThreadPool

**面试问题**：如何使用 QThreadPool？

**答案**：

```cpp
#include <QThreadPool>
#include <QRunnable>

// 方式 1：继承 QRunnable
class Task : public QRunnable {
public:
    Task(int id) : m_id(id) {}

    void run() override {
        qDebug() << "Task" << m_id << "running in thread:"
                 << QThread::currentThreadId();
        QThread::sleep(1);
    }

private:
    int m_id;
};

void threadPoolDemo() {
    QThreadPool* pool = QThreadPool::globalInstance();

    // 配置线程池
    pool->setMaxThreadCount(4);
    qDebug() << "Max threads:" << pool->maxThreadCount();
    qDebug() << "Active threads:" << pool->activeThreadCount();

    // 提交任务
    for (int i = 0; i < 10; ++i) {
        Task* task = new Task(i);
        task->setAutoDelete(true);  // 默认 true，执行后自动删除
        pool->start(task);
    }

    // 等待所有任务完成
    pool->waitForDone();
}

// 方式 2：使用 lambda（Qt 5.15+）
void lambdaThreadPool() {
    QThreadPool::globalInstance()->start([]() {
        qDebug() << "Lambda task in thread:" << QThread::currentThreadId();
    });
}

// 方式 3：带优先级
void priorityTasks() {
    QThreadPool* pool = QThreadPool::globalInstance();

    // 高优先级任务
    pool->start(new Task(1), QThread::HighPriority);

    // 低优先级任务
    pool->start(new Task(2), QThread::LowPriority);
}

// 自定义线程池
void customThreadPool() {
    QThreadPool customPool;
    customPool.setMaxThreadCount(2);
    customPool.setExpiryTimeout(30000);  // 30秒后空闲线程过期

    // 使用自定义池
    customPool.start(new Task(1));

    customPool.waitForDone();
}
```

---

### 4.2 可取消的 QRunnable

**面试问题**：如何实现可取消的线程池任务？

**答案**：

```cpp
class CancellableTask : public QObject, public QRunnable {
    Q_OBJECT

public:
    CancellableTask() {
        setAutoDelete(false);  // 手动管理生命周期
    }

    void cancel() {
        m_cancelled.storeRelease(true);
    }

    void run() override {
        emit started();

        for (int i = 0; i < 100; ++i) {
            if (m_cancelled.loadAcquire()) {
                emit cancelled();
                return;
            }

            // 执行工作
            QThread::msleep(50);
            emit progress(i);
        }

        emit finished();
    }

signals:
    void started();
    void progress(int value);
    void finished();
    void cancelled();

private:
    QAtomicInt m_cancelled{false};
};

// 使用
class TaskManager : public QObject {
    Q_OBJECT

public:
    void startTask() {
        m_task = new CancellableTask();

        connect(m_task, &CancellableTask::progress,
                this, &TaskManager::onProgress);
        connect(m_task, &CancellableTask::finished,
                this, &TaskManager::onFinished);
        connect(m_task, &CancellableTask::cancelled,
                this, &TaskManager::onCancelled);

        QThreadPool::globalInstance()->start(m_task);
    }

    void cancelTask() {
        if (m_task) {
            m_task->cancel();
        }
    }

private slots:
    void onProgress(int value) { /* ... */ }
    void onFinished() {
        m_task->deleteLater();
        m_task = nullptr;
    }
    void onCancelled() {
        m_task->deleteLater();
        m_task = nullptr;
    }

private:
    CancellableTask* m_task = nullptr;
};
```

---

## 5. 跨线程通信

### 5.1 信号槽跨线程

**面试问题**：跨线程信号槽的连接类型有哪些？

**答案**：

```cpp
// 连接类型详解
void connectionTypes() {
    QThread* thread = new QThread();
    Worker* worker = new Worker();
    worker->moveToThread(thread);

    // 1. Qt::AutoConnection（默认）
    // 同线程：DirectConnection
    // 跨线程：QueuedConnection
    connect(this, &Controller::startWork,
            worker, &Worker::doWork);

    // 2. Qt::DirectConnection
    // 槽在发射信号的线程中执行（同步）
    // 跨线程使用不安全！
    connect(this, &Controller::directSignal,
            worker, &Worker::doWork,
            Qt::DirectConnection);

    // 3. Qt::QueuedConnection
    // 槽在接收者所在线程执行（异步）
    // 参数被复制并入队
    connect(this, &Controller::queuedSignal,
            worker, &Worker::doWork,
            Qt::QueuedConnection);

    // 4. Qt::BlockingQueuedConnection
    // 像 Queued，但发送者阻塞等待
    // 危险：可能死锁！
    connect(this, &Controller::blockingSignal,
            worker, &Worker::syncOperation,
            Qt::BlockingQueuedConnection);

    // 5. Qt::UniqueConnection
    // 防止重复连接
    connect(this, &Controller::signal,
            worker, &Worker::doWork,
            Qt::QueuedConnection | Qt::UniqueConnection);
}

// 注意：QueuedConnection 的参数必须是可复制的
// 或注册为 Qt 元类型
struct CustomData {
    int id;
    QString name;
};
Q_DECLARE_METATYPE(CustomData)

void registerTypes() {
    // 注册类型用于队列连接
    qRegisterMetaType<CustomData>("CustomData");
}
```

---

### 5.2 线程安全的数据共享

**面试问题**：Qt 中有哪些线程安全的数据共享方式？

**答案**：

```cpp
// 1. 信号槽（复制数据）
class DataProducer : public QObject {
    Q_OBJECT
signals:
    void dataReady(const QByteArray& data);  // 数据被复制
};

// 2. QSharedPointer + 原子引用计数
class SharedDataExample : public QObject {
    Q_OBJECT

public:
    void processData() {
        auto data = QSharedPointer<LargeData>::create();
        // 填充数据...

        emit dataReady(data);  // 共享所有权，不复制数据
    }

signals:
    void dataReady(QSharedPointer<LargeData> data);
};

// 3. QAtomicInt / QAtomicPointer
class AtomicExample {
public:
    void increment() {
        m_counter.fetchAndAddRelaxed(1);
    }

    int value() const {
        return m_counter.loadRelaxed();
    }

    void setData(Data* newData) {
        Data* old = m_data.fetchAndStoreRelease(newData);
        delete old;
    }

private:
    QAtomicInt m_counter{0};
    QAtomicPointer<Data> m_data{nullptr};
};

// 4. 隐式共享（Copy-on-Write）
void implicitSharing() {
    QString s1 = "Hello";
    QString s2 = s1;  // 共享数据，不复制

    // 修改时才复制
    s2[0] = 'h';  // 现在 s2 有自己的数据

    // Qt 容器都是隐式共享的：
    // QString, QByteArray, QList, QVector, QHash, QMap...
    // 注意：隐式共享本身是线程安全的
    // 但容器的修改操作不是线程安全的
}
```

---

## 6. 常见面试题

### Q1: 为什么推荐 moveToThread 而不是继承 QThread？

**答案**：

```cpp
// 继承 QThread 的问题：

class BadThread : public QThread {
    Q_OBJECT

public:
    BadThread() {
        // 注意：这里的 this 属于创建 BadThread 的线程（通常是主线程）
        // 不是 run() 执行的线程！
        m_timer = new QTimer(this);  // 定时器属于主线程
    }

protected:
    void run() override {
        // 这个槽可能不会在预期的线程执行
        connect(m_timer, &QTimer::timeout, this, &BadThread::onTimeout);
        m_timer->start(1000);  // 可能报错！定时器不能跨线程启动

        exec();
    }

private slots:
    void onTimeout() {
        // 这个槽在哪个线程执行？不确定！
    }

private:
    QTimer* m_timer;
};

// moveToThread 的正确做法：

class GoodWorker : public QObject {
    Q_OBJECT

public:
    GoodWorker() {
        m_timer = new QTimer(this);  // timer 跟随 worker
        connect(m_timer, &QTimer::timeout, this, &GoodWorker::onTimeout);
    }

public slots:
    void start() {
        m_timer->start(1000);  // 在正确的线程启动
    }

private slots:
    void onTimeout() {
        // 在 worker 所在线程执行
    }

private:
    QTimer* m_timer;
};

// 使用
QThread* thread = new QThread();
GoodWorker* worker = new GoodWorker();
worker->moveToThread(thread);  // worker 和 timer 都移到新线程
connect(thread, &QThread::started, worker, &GoodWorker::start);
thread->start();
```

---

### Q2: 如何安全地从工作线程更新 GUI？

**答案**：

```cpp
// 错误：直接在工作线程更新 GUI
void badUpdate(QLabel* label) {
    QtConcurrent::run([label]() {
        // 危险！可能崩溃
        label->setText("Updated");
    });
}

// 正确方式 1：信号槽
class Worker : public QObject {
    Q_OBJECT
signals:
    void updateText(const QString& text);
};

// 主线程连接
connect(worker, &Worker::updateText, label, &QLabel::setText);

// 正确方式 2：QMetaObject::invokeMethod
void safeUpdate(QLabel* label) {
    QtConcurrent::run([label]() {
        QString result = computeResult();

        QMetaObject::invokeMethod(label, "setText",
                                  Qt::QueuedConnection,
                                  Q_ARG(QString, result));

        // 或使用 lambda（Qt 5.10+）
        QMetaObject::invokeMethod(label, [label, result]() {
            label->setText(result);
        }, Qt::QueuedConnection);
    });
}

// 正确方式 3：postEvent
class UpdateEvent : public QEvent {
public:
    static const QEvent::Type Type =
        static_cast<QEvent::Type>(QEvent::User + 1);

    UpdateEvent(const QString& text)
        : QEvent(Type), m_text(text) {}

    QString text() const { return m_text; }

private:
    QString m_text;
};

// 从工作线程发送
QCoreApplication::postEvent(widget, new UpdateEvent("New text"));
```

---

### Q3: QMutex 和 std::mutex 有什么区别？

**答案**：

| 特性 | QMutex | std::mutex |
|------|--------|------------|
| 递归锁 | QRecursiveMutex | std::recursive_mutex |
| RAII 锁 | QMutexLocker | std::lock_guard / std::unique_lock |
| 超时锁 | tryLock(timeout) | try_lock_for() |
| 可移植性 | Qt 平台 | C++11 标准 |
| 与 Qt 集成 | 原生 | 需要额外处理 |

```cpp
// Qt 风格
QMutex mutex;
QMutexLocker locker(&mutex);

// C++ 风格
std::mutex mutex;
std::lock_guard<std::mutex> lock(mutex);

// Qt 推荐在 Qt 项目中使用 QMutex
// 以保持代码风格一致性
```

---

### Q4: Qt 线程中如何处理异常？

**答案**：

```cpp
// Qt 信号槽不传播异常
// 需要手动处理

class SafeWorker : public QObject {
    Q_OBJECT

public slots:
    void doWork() {
        try {
            riskyOperation();
            emit finished();
        } catch (const std::exception& e) {
            emit error(QString::fromStdString(e.what()));
        } catch (...) {
            emit error("Unknown error");
        }
    }

signals:
    void finished();
    void error(const QString& message);
};

// QtConcurrent 中的异常
void handleConcurrentException() {
    QFuture<void> future = QtConcurrent::run([]() {
        throw std::runtime_error("Something went wrong");
    });

    try {
        future.waitForFinished();
        // 检查异常
        if (future.isCanceled()) {
            // 被取消
        }
    } catch (const QException& e) {
        qDebug() << "Caught:" << e.what();
    } catch (const std::exception& e) {
        qDebug() << "Caught std:" << e.what();
    }

    // 或使用 watcher
    QFutureWatcher<void>* watcher = new QFutureWatcher<void>();
    // 注意：finished 信号在异常时也会发射
    // 需要在槽中检查异常
}
```

---

## 总结

Qt 多线程的核心要点：

1. **moveToThread** - 推荐的工作线程模式
2. **同步原语** - QMutex、QReadWriteLock、QWaitCondition、QSemaphore
3. **Qt Concurrent** - 高级并行编程
4. **QThreadPool** - 线程池管理
5. **信号槽** - 线程安全的跨线程通信
6. **GUI 线程** - 只在主线程操作 GUI

遵循这些原则可以编写出安全、高效的多线程 Qt 应用程序。
