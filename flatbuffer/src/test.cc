#include "stu_generated.h"
#include <iostream>
#include <vector>
#include <string>
using namespace std;

/*******************
 * 说明：
 * 
 * 1. 基础类型直接赋值，复合类型需要用 builder.CreateXXX，例如String和Vector
 * 2. Struct类型，直接构造
 * 3. Table类型，用CreateXXX，其中XXX为定义的类型
 * 4. 序列化时候，可以直接从Builder取出指针和length，然后就可以傳輸了。
 * 5. 反序列化的时候，注意所有成员都需要用函数()而不是直接使用成员名。
 * 6. 可以序列化和反序列化不是root_type的結构，注意反序列時用 flatbuffers::GetRoot<XXX>(ptr)
*********************/

void test1()
{
  ////////// Serialize //////////

  flatbuffers::FlatBufferBuilder builder;

  vector<uint32_t> vec = {1, 2, 3, 4, 5, 6};

  auto name = builder.CreateString("eric");
  auto id = 1000;
  auto mylist = builder.CreateVector(vec);
  auto pos = MyFb::POS(1, 3);
  auto obj = MyFb::CreateMyObj(builder, name, id, mylist, &pos);
  builder.Finish(obj);

  uint8_t *ptr = builder.GetBufferPointer();
  uint32_t len = builder.GetSize();
  cout << "Serialize: len=" << len << endl;


  ////////// Deserialize //////////
  auto oo = MyFb::GetMyObj(ptr);

  cout << "name: " << oo->name()->str() << endl;
  cout << "id: "   << oo->id()   << endl;
  for (int i = 0; i < oo->mylist()->size(); ++i) {
    cout << "mylist: "   << oo->mylist()->Get(i) << endl;
  }
  cout << "pos: " << oo->pos()->x() << "  " << oo->pos()->y() << endl;
}

void test2()
{
  ////////// Serialize //////////
  flatbuffers::FlatBufferBuilder builder;
  auto request = MyFb::CreateMyObj2(builder, 
        builder.CreateString("erick huang"), 100);
  builder.Finish(request);

  uint8_t *ptr = builder.GetBufferPointer();
  uint32_t len = builder.GetSize();
  cout << "Serialize: len=" << len << endl;

  ////////// Deserialize //////////
  auto response = flatbuffers::GetRoot<MyFb::MyObj2>(ptr);
  cout << "name: " << response->name()->str() << endl;
  cout << "id: "   << response->id()   << endl;
}

int main(int argc, char *argv[])
{
  test1();
  test2();
  return 0;
}