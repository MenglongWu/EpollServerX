/**
* @brief epoll server对象实例ddddff
 \example a.c
 * This is an example of how to use the struct ep_t class.
 * More details about this example.
 */
 void main()
{
  Test t;
  t.example();
}



文档规范
代码文档遵循对“对外文档强制统一，个人文档保存统一”即用于多人开发的项目，相互之间并不关心对方是如何实现的，只要说明清楚接口怎么使用，接口风格强制项目组保存统一。对于源代码里接口代码编写规范的不苛刻强调，只要求个人保持统一。\n
本工程里所有代码采用doxygen所支持的注释规范，保证对外文档规格统一。\n
至于代码编写规范建议参阅《C++/C高质量编程》

文件标题
每个文件必须包含文件说明，说明内容包括文件名，实现功能，调试平台（操作系统、依赖库）、版本、作者、修订时间、修订内容、版权声明

/**
 ******************************************************************************
 * @file	
 * @brief	
 *		TODO:brief 

 	TODO:introduce 
 *
*/
/*--------------------------------------------------
 * version    |    author    |    date    |    content
 * V1.0 		Nane		20xx-xx-xx	1.xxxxx
 ******************************************************************************
 * @attention
 *
 * ATTENTION
 *
 * <h2><center>&copy; COPYRIGHT </center></h2>
 *****************************************************************************
*/
函数声明
函数声明包括函数功能、参数意义、返回值、注意事项，如果可能请在别留下函数的使用例程
/**
 * @brief	
 * @param\n
 * @retval\n	NULL
 * @remarks	
 */

数据类型
为保证可移植性，建议在基础类型上采用别名方式，否则默认工程适用于32为小端头CPU，并且整个工程一个数据类型只有一种命名
typedef char byte 
typedef char uint8 
typedef unsigned short uint16 
一旦采用如上命名方式，则代码里除非采用其他库函数必须采用原始数据类型(有可能编译器提示警告），否则不要出现将别名和原始数据类型混用
//不允许
char c;
byte b;
b = c;
//允许
char c;
byte b;
scanf("%c",&c);

命名规则
采用由意义的英文全称或缩写，函数、宏定义前为了避免与其他库命名冲突，可在名字前加前缀，前缀能体现该库特有即可，无特殊要求
//OpenGL命名前缀
glBegain()
//pthread库命名前缀
pthread_create()
pthread_mutex_lock()

宏定义
全部采用有意义的英文大写，单词之间用"_"隔开
#define MAX_LEN		(1024)
#define OS_SUCCESS	(0)
#define OS_ERROR	(-1)
函数
采用英文词组组合，两词组用大写字母隔开，函数命名建议采用动词+名称组合
int GetPassword();
int lib_GetPassword();//带前缀

变量
函数内变量命名风格不做强制要求，只需要风格统一，如下是函数内命名风格建议
全部采用有意义的英文，第一个英文字母小写，单词之间用大写字母隔开。指针变量加前缀p，二级指针加前缀pp，变量名建议采用形容词+名称
char userName[20];
int  currentTime;
char *puserName;
char **ppnextPoint;


作用域命名规则
作用域命关系到变量生存期命名规则属于强制执行。局部静态变量加前缀"s_"，全局变量加前缀"g_"，线程共享变量加前缀"gg_"，全局变量、函数加前缀双下划线"__"，
int gg_threadShare;		//整个工程可以访问
int __g_only;			//只限该c文件访问

static void __fun()		//只限该c文件访问
{
	//....
}
void fun()
{
	static int s_lastTimes = 0;//只有该函数访问
	//....
	__fun();
}
结构体
采用英文词组组合，两词组用"_"隔开，词组全部用小写。
struct student_inf
{
	int      age;
	char[20] name;
	char[20] father_name;
	char[20] mather_name;
};
不建议在结构体上滥用typedef重命名struct类型，这样除了代码书写偷懒之外没有其他好处，建议typedef重命名的数据类型后加后缀"_t"，同理枚举加后缀"_e"、联合体加后缀"_u"
typedef struct student_inf student_inf_t;//不建议

typedef int HPEN   //允许
typedef int HBRUSH //允许
typedef int HFONT  //允许

enum errcode_e
{
	code1 = 1;
	codd2 = 2;
};

union data_u
{
	int  data;
	void *ptr;
};

