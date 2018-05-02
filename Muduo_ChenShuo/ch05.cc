#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>
#include <muduo/base/Timestamp.h>

#include <stdio.h>
#include <sys/resource.h>
#include <unistd.h>

#include <muduo/base/LogStream.h>

int main(int argc, char* argv[])
{

    {//Logging
        //#define LOG_INFO \
        //if (muduo::Logger::logLevel() <= muduo::Logger::INFO) \
        //muduo::Logger(__FILE__, __LINE__).stream()
        LOG_INFO << "EventLoop created ";
        // 使用s+fin大发调试可以发现：
        // 秘密都藏在muduo::Logger类的析构函数中
        /*
           Logger::~Logger()
           {
               impl_.finish();
               const LogStream::Buffer& buf(stream().buffer());
               g_output(buf.data(), buf.length());
               if (impl_.level_ == FATAL)
               {
                   g_flush();
                   abort();
               }
           }
           析构函数调用g_output()这个函数调用defaultOutput(),
           将buf中的数据打印到屏幕！
           muduo::Logger.stream_成员是一个muduo::LogStream类对象，
           muduo::LogStream类仅仅是一个提供了>>操作符的buffer!
       */
    }

    {//AsyncLogging
        off_t kRollSize = 500*1000*1000;
        static muduo::AsyncLogging* g_asyncLog = NULL;
        {
            // set max virtual memory to 2GB.
            size_t kOneGB = 1000*1024*1024;
            rlimit rl = { 2*kOneGB, 2*kOneGB };
            setrlimit(RLIMIT_AS, &rl);
        }

        printf("pid = %d\n", getpid());

        char name[256] = { 0 };
        strncpy(name, argv[0], sizeof name - 1);
        muduo::AsyncLogging log(::basename(name), kRollSize);
        log.start();//log线程负责将缓冲区的数据写入文件
        g_asyncLog = &log;

        bool longLog = argc > 1;

        {//负责向缓冲区写入数据
            //g_asyncLog保存了文件信息，因此向文件写log数据
            muduo::Logger::setOutput([](const char* msg, int len)
                    {g_asyncLog->append(msg, len);});

            int cnt = 0;
            const int kBatch = 1000;
            muduo::string empty = " ";
            muduo::string longStr(3000, 'X');
            longStr += " ";

            for (int t = 0; t < 30; ++t)
            {
                muduo::Timestamp start = muduo::Timestamp::now();
                for (int i = 0; i < kBatch; ++i)
                {
                    //Logger析构时，将log数据(保存在buffer_)
                    //输出到setOutput函数设定的lambda
                    LOG_INFO << "Hello 0123456789" 
                        << " abcdefghijklmnopqrstuvwxyz "
                        << (longLog ? longStr : empty)
                        << cnt;
                    ++cnt;
                }
                muduo::Timestamp end = muduo::Timestamp::now();
                printf("%f\n", timeDifference(end, start)*1000000/kBatch);
                struct timespec ts = { 0, 500*1000*1000 };
                nanosleep(&ts, NULL);
            }
        }
    }
}
