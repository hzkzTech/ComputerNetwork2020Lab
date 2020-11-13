<center><font size=8 face="楷体">计算机网络实验二</font></center>

<center><font size=5 face="楷体">Web服务器配置，HTTP报文捕获</font></center>



<center><font face="楷体">作者：鲁含章</font></center>

<center><font face="楷体">学号：1811398</font></center>

<center><font face="楷体">日期：2020年11月10日</font></center>

# 目录

[TOC]

# 一、实验要求

1. 搭建Web服务器（自由选择系统），并制作简单Web页面，包含简单文本信息（至少包含专业、学号、姓名）。
2. 通过浏览器获取自己编写的Web页面，使用Wireshark捕获与Web服务器的交互过程，并进行简单分析说明。

# 二、实验内容及结果

## （一）搭建web服务器，制作简单Web页面

web页面中加入**图片、音频、视频**做不同传输测试。

```html
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>抓包测试</title>
<style>
.center {
    margin: auto;
    width: 80%;
    border: 3px solid black;
    padding: 10px;
}
</style>
</head>
 
<body>
    <div class="center" align="center" style="color:ivory;background-color:black;">
        <h1>计算机网络抓包测试网页</h1>
        <h3>鲁含章 1811398</h3>
    </div>
    <div class="center" style="color:black;background-color:peachpuff;">
        <h2>English test</h2>
            This is English.

        <h2>汉字测试</h2>
            这是一段汉字。</br>

        <h2>图片测试</h2>
            <img src="source/1.jpg" alt="" width="600">

        <h2>音频测试</h2>
            <audio controls="controls">
                <source src="source/test.mp3" type="audio/mpeg">
            Your browser does not support the audio tag.
            </audio>

        <h2>视频测试</h2>
            <video width="520" height="400" controls="controls" loop="loop">
                <source src="source/movie.mp4" type="video/mp4">
            Your browser does not support the video tag.
            </video>
    
    </div>
</body>
</html>
```
因为windows下该版本的wireshark无法直接抓取本地回环数据包，故将网页搭载到github的远程服务器，访问https://hzkztech.github.io/ 可以得到如下测试页面。

![image-20201113202501845](C:\Users\98140\AppData\Roaming\Typora\typora-user-images\image-20201113202501845.png)

## （二）使用Wireshark捕获交互过程
访问https://hzkztech.github.io/ ，该域名被解析为ip地址185.199.109.153。本站点使用https协议，其中对于数据的传输是TLSv1.2 协议的加密方式，通过wireshark无法直接获取确定内容，不过可以观察其在不同用户端动作之后的交互过程。

对使用 wireshark 捕获结果，使用`ip.addr == 185.199.109.153`过滤得到与本相关其中编号78以前是握手阶段，78号之后是握手后双方使用商议好的秘钥进行通讯。

![image-20201113175752378](C:\Users\98140\AppData\Roaming\Typora\typora-user-images\image-20201113175752378.png)

### 1. tcp/ip 三次握手

![image-20201113194116540](C:\Users\98140\AppData\Roaming\Typora\typora-user-images\image-20201113194116540.png)

60：客户端向 443 端口发送 SYN 信号
66：服务端回应连接
68：tcp/ip 三次握手完成

### 2. TLS 握手过程

![image-20201113194203449](C:\Users\98140\AppData\Roaming\Typora\typora-user-images\image-20201113194203449.png)

69：Client Hello 信号，客户端发送随机数字 + 自己可以支持的加密方法。
73：Server Hello 信号，服务器发送随机数字 + 选择双方都支持的加密方式，这里选择的是`TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256`。

<img src="C:\Users\98140\AppData\Roaming\Typora\typora-user-images\image-20201113182007646.png" alt="image-20201113182007646" style="zoom: 33%;" />

74：ACK 应答
76：发送自己的公钥和从 CA 申请的证书

<img src="C:\Users\98140\AppData\Roaming\Typora\typora-user-images\image-20201113194915141.png" alt="image-20201113194915141" style="zoom:33%;" />

79，80：客户端发送的请求头（数据经过加密）
84：服务端用密文形式发送的 html 文档

### 3. 数据传输过程TCP+TLSv1.2

**测试图片、音频、视频等文件的传输**

服务器发来的数据都经过加密的，无法解析出具体内容。

在抓取的包中观察到如下几类交互过程。不同于小size的文本，这些文件在传输时一次TCP段长度（MSS）是不够的，需要多次传输，而这期间客户端只回应ACK。

**（1）正常传输过程：**

注意到这里客户端发送的Seq始终为0，这是因为客户端每次只发送ACK报文，`TCP Segment Len: 0`，`Sequence number`与`Next Sequence number`相同，服务器每次反馈的ACK也便相同。这种情况在客户端发送具体请求后变化，例如当按下视频播放按钮，请求服务器发送后续的视频内容时，`TCP Segment Len`变为有效值。

![image-20201113175956443](C:\Users\98140\AppData\Roaming\Typora\typora-user-images\image-20201113175956443.png)

服务器发来的头部包含如下信息：

![image-20201113201214524](C:\Users\98140\AppData\Roaming\Typora\typora-user-images\image-20201113201214524.png)

**（2）失序处理：**

重复发送ACK

![image-20201113195550941](C:\Users\98140\AppData\Roaming\Typora\typora-user-images\image-20201113195550941.png)
**（3）冗余包：**

遇到冗余包会重复发送ACK[冗余包的next seq]

![image-20201113200322133](C:\Users\98140\AppData\Roaming\Typora\typora-user-images\image-20201113200322133.png)

### 4. TCP四次挥手

由于TCP连接是全双工的，因此每个方向都必须单独进行关闭。

19：TCP客户端发送一个FIN，用来关闭客户到服务器的数据传送
22：服务器收到这个FIN，它发回一个ACK，确认序号为收到的序号加1
22：服务器关闭客户端的连接，发送一个FIN给客户端
24：客户端发回ACK报文确认，并将确认序号设置为收到序号加1

![image-20201113192846545](C:\Users\98140\AppData\Roaming\Typora\typora-user-images\image-20201113192846545.png)

### 5. 其他现象——chrome的双端口连接

在TCP握手时，可以观察到本地同时发出了两个不同端口的连接请求——`6158`与`6159`端口，其中6158是之后持续使用的端口，而6159端口在不久之后就由客户端断开连接（见78，81，85）。经搜索，这是许多浏览器的一种加速访问策略：一次建立两个连接，然后使用其中一个继续通信，避免有一个连接失败的情况。

![image-20201113175752378](C:\Users\98140\AppData\Roaming\Typora\typora-user-images\image-20201113175752378.png)

# 三、总结

在本次实验中，实现了包括文本、图片、音频和视频的测试html页面，搭载到web服务器上，并通过Wireshark抓取客户端与服务器段的交互过程，观察到TCP握手/挥手过程、TLS握手/传输过程以及对一些异常情况的处理。

通过本次实验，对TCP/IP协议的过程有了更深层次的理解，为后续的实验做好了准备。

<h3><center>参考文献</center></h3>

[1]  https://blog.csdn.net/lblblblblzdx/article/details/88684788 **TLSv1.2 协议了解**

[2] https://www.cnblogs.com/monkey0307/p/9675123.html **SSL/TSL 握手过程详解**