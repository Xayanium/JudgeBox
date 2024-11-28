## 使用c语言作为判题核心，python语言编写判题机，支持docker一键部署

### 本判题机使用NATS作为消息队列，接受后端传入的判题请求，并通过NATS返回判题结果给后端；使用MinIO作为对象存储服务，存储判题数据

```python
# 后端传递的数据包括: 判题id, 题目id, 用户代码及所用语言, 判题机的时空限制, 是否为特判
                    judge_json = {
                        'judge_id': (int),
                        'problem_id': (int),
                        'problem_code': (string),
                        'language': (string),
                        'code': (string),
                        'time_limit': (int),
                        'memory_limit': (int)
                    }
# 判题结果json数据, 传回给后端
                        result_json = {
                            'judge_id': self.judge_id,
                            'case_id': 0,
                            'time_cost': 0,
                            'memory_cost': 0,
                            'result': '',
                            'message': '',
                            'input_data': '',
                            'sample_output': '',
                            'user_output': ''
                        }

```
### 1. 修改 `client_settings.json` 配置文件信息
### 2. 在项目根目录下使用 `docker build -t judger .` 命令生成docker镜像
### 3. 在项目使用 `docker run -v $(pwd)/problem:/app/problem -v $(pwd)/client_settings.json:/app/client_settings.json -d --name judger judger:latest` 运行判题机

