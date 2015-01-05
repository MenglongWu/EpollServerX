/*! \mainpage A simple manual

Some general info.

* @section bug
各模块缺陷
- \subpage intro
- \subpage advanced "Advanced usage"
- \subpage ep "epollserver bug"
- \subpage mini "minishell_core bug"
- \subpage gl "glink bug"

* @section Version
各模块版本
- \subpage epver "epollserver version"
- \subpage miniver "minishell_core version"
- \subpage gl "glink version"
*/

//-----------------------------------------------------------

/*! \page intro Introduction
This page introduces the user to the topic.
Now you can proceed to the \ref advanced "advanced section".
*/

//-----------------------------------------------------------

/*! \page advanced Advanced Usage
This page is for advanced users.
Make sure you have first read \ref intro "the introduction".
*/


/*! \page ep epserver bug
This page is for advanced users.
Make sure you have first read \ref intro "fffffthe introduction".

* @section bug
	- todo more bug
	- 00x-201x-xx-xx:
	- 004-2015-04-18: bug accept后立即在pFUI_OnAccept里发送数据，导致远端“段错误”，原因由于远端
	先在pFUI_OnConnect里执行“接收缓存初始化”，如果在未完成“接收缓存初始化”，甚至为调用pFUI_Onconnect前就接收数据，将导致“段错误”，简易应用层pFUI_OnRecv自己检测是否初始化，虽然epollserver也可以检测完成pFUI_OnAccept后才允许调用pFUI_OnRecv，但不是epollserver的义务
	- 003-2015-04-03: fix 诸多epollserver回调pFUI_xxx函数输入参数全是0问题，之前只留下接口没有具体测试，故意将接口参数全是0，出现bug后方便差错
	- 002-2015-03-19: bug 连接成功返回0但是仍然显示fd无效Connect: Bad file descriptor
	- 001-2015-03-19: fix ep_Connect不返回sockfd
*/

/*! \page mini minishell_core bug
This page is for advanced users. minishell
Make sure you have first read \ref intro "fffffthe introduction".

* @section bug
	- todo more bug
	- 00x-201x-xx-xx:
*/

/*! \page gl glink bug
This page is for advanced users. glink
Make sure you have first read \ref intro "fffffthe introduction".

* @section bug
	- todo more bug
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
	- 00x-201x-xx-xx:
*/



/*! \page epver epollserver version
This page is for advanced users. ep
*/

/*! \page miniver minishell_core version
This page is for advanced users. minishell
*/
/*! \page gl glink version
This page is for advanced users. glink
@section version
	- This page is for advanced users. glink
	- This page is for advanced users. glink
	- This page is for advanced users. glink
	- This page is for advanced users. glink
	- This page is for advanced users. glink
	- This page is for advanced users. glink
	- This page is for advanced users. glink
	- This page is for advanced users. glink
		- This page is for advanced users. glink
		- This page is for advanced users. glink
		- This page is for advanced users. glink
		- This page is for advanced users. glin
	- This page is for advanced users. glink
	- This page is for advanced users. glink
*/
