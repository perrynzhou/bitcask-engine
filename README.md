# bitcask-engine


### 设计思想

- 每个schema保存的art树，叶子节点保存了所有key-value所在文件的偏移量，内存中保存的都是schema中最新的数据，每个schema中磁盘文件中的key-value都是以`append`方式进行写入，所以后续需要做`compaction`.

### ART树介绍

![](./doc/art.png)
### 编译bitcask库

```shell
$ git clone git@github.com:perrynzhou/bitcask-engine.git && cd src  && make clean && make 
```

### 编译bitcask测试

```shell
$ cd test && make clean && make
```

## 未来计划

![](./doc/bitcask-todo.png)