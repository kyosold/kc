# kc

#### 介绍
c语言扩展

#### 软件架构
封装了一些常用的库
- klog: 日志
- klist: 链表list, 追加多个 char * 使用
- kcodec: 编解码
    - base64
    - url/url raw
- kcrypto: 
    - hash计算:
        - md4
        - md5
        - sha1
        - sha224
        - sha256
        - sha384
        - sha512
    - rsa:
        - public encode
        - private decode
        - private encode
        - public decode
    - aes:
        - encode
        - decode
- kcurl: 封装 libcurl 方法(get, post, put)
- kmc: 封装 libmemcached 方法(set, get, del)
- kmysql: 封装 mysql 方法(exec, query, trans)
- kchar: 封装 char 操作方法(追加、拷贝)
- klist: 字典， key -> value



#### 安装教程

1. 需要哪个功能，直接把源文件加到工程中使用就行。
2. 有些需要添加链接库如:
    - kcrypto 需要 OpenSSL
    - kmc 需要 libmemcached
    - kmysql 需要 mariadb-devel
    - kcurl 需要 libcurl-devel

#### 使用说明

1.  使用说明在每个功能里都有


#### 参与贡献

1.  Fork 本仓库
2.  新建 Feat_xxx 分支
3.  提交代码
4.  新建 Pull Request


#### 特技

1.  使用 Readme\_XXX.md 来支持不同的语言，例如 Readme\_en.md, Readme\_zh.md
2.  Gitee 官方博客 [blog.gitee.com](https://blog.gitee.com)
3.  你可以 [https://gitee.com/explore](https://gitee.com/explore) 这个地址来了解 Gitee 上的优秀开源项目
4.  [GVP](https://gitee.com/gvp) 全称是 Gitee 最有价值开源项目，是综合评定出的优秀开源项目
5.  Gitee 官方提供的使用手册 [https://gitee.com/help](https://gitee.com/help)
6.  Gitee 封面人物是一档用来展示 Gitee 会员风采的栏目 [https://gitee.com/gitee-stars/](https://gitee.com/gitee-stars/)
