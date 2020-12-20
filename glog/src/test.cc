#include <iostream>
#include <glog/logging.h>
using namespace std;

// 备注：
// 1. 第一次安装glog后，如果运行程序时找不到glog动态库，可以执行如下命令更新一下配置：
//    # sudo /sbin/ldconfig -v
// 2. 打印无需加endl，会自动环行；
// 3. 只支持按照文件大小分日志；

void test()
{
  LOG(INFO) << "hello glog";
  LOG(WARNING) << "hello glog";
  LOG(ERROR) << "hello glog";
  LOG(FATAL) << "hello glog";
}

int main(int argc, char *argv[])
{
  FLAGS_log_dir = "./";  //设置日志文件保存目录，这个目录必须是已经存在的，否则不能生成日志文件。
  google::InitGoogleLogging(argv[0]); //使用glog之前必须先初始化库, argv[0]是程序名

  FLAGS_logtostderr = false;  //设置日志消息是否转到标准输出而不是日志文件

  FLAGS_alsologtostderr = true;  //设置日志消息除了日志文件之外是否去标准输出

  FLAGS_colorlogtostderr = true;  //设置记录到标准输出的颜色消息（如果终端支持）

  FLAGS_log_prefix = true;  //设置日志前缀是否应该添加到每行输出

  FLAGS_logbufsecs = 0;  //设置可以缓冲日志的最大秒数，0指实时输出

  FLAGS_max_log_size = 10;  //设置最大日志文件大小（以MB为单位）

  FLAGS_minloglevel = google::GLOG_INFO;

  FLAGS_logbuflevel = google::GLOG_INFO;

  FLAGS_stop_logging_if_full_disk = true;  //设置是否在磁盘已满时避免日志记录到磁盘

  if (!FLAGS_logtostderr) {
    //设置特定严重级别的日志的输出目录和前缀。第一个参数为日志级别，第二个参数表示输出目录及日志文件名前缀
    google::SetLogDestination(google::GLOG_INFO, "./myinfo_");
    google::SetLogDestination(google::GLOG_WARNING, "./mywarning_");
    google::SetLogDestination(google::GLOG_ERROR, "./myerror_");
    google::SetLogDestination(google::GLOG_FATAL, "./myfatal_");
    
    google::SetLogFilenameExtension("xxx_");  //在日志文件名中级别后添加一个扩展名。适用于所有严重级别
  }

  test();

  google::ShutdownGoogleLogging(); //当要结束glog时必须关闭库，否则会内存溢出：
  return 0;
}