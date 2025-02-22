# 抓取臺股證交所每日股價資料，儲存到MongoDB


## 1. MongoDB安裝

```sh
sudo apt-get install mongodb
```

###查看是否安裝成功，查看版本

```sh
mongo -version
```

### 啟動/關閉 mongoDB數據庫服務的命令

```sh
service mongodb start
service mongodb stop
```

### mongo啟動shell模式

```sh
mongo
```


## 2. python調用mongo的驅動安裝

### 安裝pymongo驅動

```sh
pip install pymongo
```

### 更新pymongo驅動

```sh
pip install --upgrade pip
```

## 3. 測試


```py
#!/usr/bin/env python
# -*- coding:utf-8 -*-

from pymongo import MongoClient

conn = MongoClient('127.0.0.1', 27017)
db = conn.mydb  
my_set = db.test_set

my_set.insert({"name":"joe001","age":3})
my_set.insert({"name":"joe002","age":4})
my_set.insert({"name":"joe003","age":5})

for i in my_set.find():
    print(i)

```


## 好用的 MongoDB GUI manager

```sh
https://robomongo.org/
```

### Robomongo — 好用的 MongoDB GUI manager

```sh
https://medium.com/@wilsonhuang/robomongo-%E5%A5%BD%E7%94%A8%E7%9A%84-mongodb-gui-manager-87508da806e5

```

--- 

將股票交易資料放進MongoDB資料庫，就不用需要每次從證交所讀取資料

從網址http://www.twse.com.tw/exchangeReport/STOCK_DAY?date=20180817&stockNo=2330讀取股票交易資料，該網址會以JSON格式回傳股票編號2330指定日期20180817的一個月股票交易資料，再進行處理放進MongoDB資料庫。

本程式在GitHub的網址
https://github.com/jang0820/Stock/blob/master/FromTwseToMongo.py


### Step1)須先建立接收的MongoDB資料庫，設定好主機位址、資料庫名稱與資料表名稱，使用pymongo進行連線資料庫。


```py
import pymongo
MONGO_HOST = 'localhost'
MONGO_DB = 'TwStock'
MONGO_COLLETION = 'twse'
from pymongo import MongoClient
#   http://www.twse.com.tw/exchangeReport/STOCK_DAY?date=20180817&stockNo=2330

def connect_mongo():  #連線資料庫
    global collection
    client = MongoClient(MONGO_HOST, 27017)
    db = client[MONGO_DB]
    collection = db[MONGO_COLLETION]
    ```

### Step2)定義抓取股票交易資料函式get_stock_history

```py
def get_stock_history(date, stock_no, retry = 5):   #從www.twse.com.tw讀取資料
    quotes = []
    url = 'http://www.twse.com.tw/exchangeReport/STOCK_DAY?date=%s&stockNo=%s' % ( date, stock_no)
    r = requests.get(url)
    data = r.json()
    return transform(data['data'])  #進行資料格式轉換
```

### Step3)進行資料格式轉換再放入資料庫

```py
def transform_date(date):   #民國轉西元
        y, m, d = date.split('/')
        return str(int(y)+1911) + '/' + m  + '/' + d
    
def transform_data(data):   #將證交所獲得資料進行資料格式轉換
    data[0] = datetime.datetime.strptime(transform_date(data[0]), '%Y/%m/%d')
    data[1] = int(data[1].replace(',', ''))#把千進位的逗點去除
    data[2] = int(data[2].replace(',', ''))
    data[3] = float(data[3].replace(',', ''))
    data[4] = float(data[4].replace(',', ''))
    data[5] = float(data[5].replace(',', ''))
    data[6] = float(data[6].replace(',', ''))
    data[7] = float(0.0 if data[7].replace(',', '') == 'X0.00' else data[7].replace(',', ''))  # +/-/X表示漲/跌/不比價
    data[8] = int(data[8].replace(',', ''))
    return data

def transform(data):   #讀取每一個元素進行資料格式轉換，再產生新的串列
    return [transform_data(d) for d in data]
    ```
    
    
### Step4)配合證交所網站產生年與月的資料，查詢該月的股價

```py
def genYM(smonth, syear, emonth, eyear):  #產生從syear年smonth月到eyear年emonth月的所有年與月的tuple
    start = 12 * syear + smonth
    end = 12 * eyear + emonth
    for num in range(int(start), int(end) + 1):
        y, m = divmod(num, 12)
        yield y, m
```

### Step5)整合Step1到Step4所寫的函式，使用定義fetch_data函式抓取資料儲存到資料庫

```py
def fetch_data(year: int, month: int, stockno):  #擷取從year-month開始到目前為止的所有交易日資料
    raw_data = []
    today = datetime.datetime.today()
    for year, month in genYM(month, year, today.month, today.year): #產生year-month到今天的年與月份，用於查詢證交所股票資料
        if month < 10:
            date = str(year) + '0' + str(month) + '01'  #1到9月
        else:
            date = str(year) + str(month) + '01'   #10月
        data = get_stock_history(date, stockno)
        for item in data:  #取出每一天編號為stockno的股票資料
            if collection.find({    #找尋該交易資料是否不存在
                    "date": item[0],
                    "stockno": stockno
                } ).count() == 0:
                element={'date':item[0], 'stockno':stockno, 'shares':item[1], 'amount':item[2], 'open':item[3], 'close':item[4], 
                     'high':item[5], 'low':item[6], 'diff':item[7], 'turnover':item[8]};  #製作MongoDB的插入元素
                print(element)
                collection.insert_one(element)  #插入元素到MongoDB
        time.sleep(10)  #延遲5秒，證交所會根據IP進行流量統計，流量過大會斷線
```

### Step6)呼叫連線資料庫，使用函式fetch_data取出編號2892的股票，從201704到今天的股價與成交量資料

```py
connect_mongo()   #連線資料庫
fetch_data(2017, 4, '2892')   #取出編號2892的股票，從201704到今天的股價與成交量資料

```

### 完整程式碼如下

```py
#將股票交易資料放進MongoDB資料庫，就不用需要每次從證交所讀取資料
import numpy as np
import requests
import pandas as pd
import datetime
import json
import matplotlib.pyplot as pp
import time
import pymongo
MONGO_HOST = 'localhost'
MONGO_DB = 'TwStock'
MONGO_COLLETION = 'twse'
from pymongo import MongoClient
#   http://www.twse.com.tw/exchangeReport/STOCK_DAY?date=20180817&stockNo=2330

def connect_mongo():  #連線資料庫
    global collection
    client = MongoClient(MONGO_HOST, 27017)
    db = client[MONGO_DB]
    collection = db[MONGO_COLLETION]

def get_stock_history(date, stock_no, retry = 5):   #從www.twse.com.tw讀取資料
    quotes = []
    url = 'http://www.twse.com.tw/exchangeReport/STOCK_DAY?date=%s&stockNo=%s' % ( date, stock_no)
    print(url)
    data = ''
    while data == '':
        try:
            s = requests.session()
            s.keep_alive = False # 關閉多餘連接
            #r = requests.get(url)
            r = s.get(url)
            data = r.json()
            return transform(data['data'])  #進行資料格式轉換
        except:
            print("Connection refused by the server..")
            print("Let me sleep for 5 seconds")
            print("ZZzzzz...")
            time.sleep(5)
            print("Was a nice sleep, now let me continue...")
            continue

def transform_date(date):   #民國轉西元
        y, m, d = date.split('/')
        return str(int(y)+1911) + '/' + m  + '/' + d
    
def transform_data(data):   #將證交所獲得資料進行資料格式轉換
    data[0] = datetime.datetime.strptime(transform_date(data[0]), '%Y/%m/%d')
    data[1] = int(data[1].replace(',', ''))#把千進位的逗點去除
    data[2] = int(data[2].replace(',', ''))
    data[3] = float(data[3].replace(',', ''))
    data[4] = float(data[4].replace(',', ''))
    data[5] = float(data[5].replace(',', ''))
    data[6] = float(data[6].replace(',', ''))
    data[7] = float(0.0 if data[7].replace(',', '') == 'X0.00' else data[7].replace(',', ''))  # +/-/X表示漲/跌/不比價
    data[8] = int(data[8].replace(',', ''))
    return data

def transform(data):   #讀取每一個元素進行資料格式轉換，再產生新的串列
    return [transform_data(d) for d in data]

def genYM(smonth, syear, emonth, eyear):  #產生從syear年smonth月到eyear年emonth月的所有年與月的tuple
    start = 12 * syear + smonth
    end = 12 * eyear + emonth
    for num in range(int(start), int(end) + 1):
        y, m = divmod(num, 12)
        yield y, m

def fetch_data(year: int, month: int, stockno):  #擷取從year-month開始到目前為止的所有交易日資料
    raw_data = []
    today = datetime.datetime.today()
    for year, month in genYM(month, year, today.month, today.year): #產生year-month到今天的年與月份，用於查詢證交所股票資料
        if month < 10:
            date = str(year) + '0' + str(month) + '01'  #1到9月
        else:
            date = str(year) + str(month) + '01'   #10月
        data = get_stock_history(date, stockno)

        if data == None:
            print(data)
            continue

        for item in data:  #取出每一天編號為stockno的股票資料
            if collection.find({    #找尋該交易資料是否不存在
                    "date": item[0],
                    "stockno": stockno
                } ).count() == 0:
                element={'date':item[0], 'stockno':stockno, 'shares':item[1], 'amount':item[2], 'open':item[3], 'close':item[4], 
                     'high':item[5], 'low':item[6], 'diff':item[7], 'turnover':item[8]};  #製作MongoDB的插入元素
                print(element)
                collection.insert_one(element)  #插入元素到MongoDB
        time.sleep(10)  #延遲5秒，證交所會根據IP進行流量統計，流量過大會斷線

connect_mongo()   #連線資料庫
fetch_data(2010, 8, '2022')   #取出編號2892的股票，從201704到今天的股價與成交量資料

```

### query

```py
import pandas as pd
import datetime
import matplotlib.pyplot as pp
import pymongo
MONGO_HOST = 'localhost'
MONGO_DB = 'TwStock'
MONGO_COLLETION = 'twse'
from pymongo import MongoClient
#   http://www.twse.com.tw/exchangeReport/STOCK_DAY?date=20180817&stockNo=2330

def connect_mongo():  #連線資料庫
    global collection
    client = MongoClient(MONGO_HOST, 27017)
    db = client[MONGO_DB]
    collection = db[MONGO_COLLETION]

connect_mongo()  #呼叫連線資料庫函式
query = { 'stockno' : '2892' }
cursor = collection.find(query)  #依query查詢資料
stock =  pd.DataFrame(list(cursor))  #轉換成DataFrame
del stock['_id']  #刪除欄位_id
print(stock)
indexlist = []
for i in range(len(stock)):  #建立日期串列
    indexlist.append(stock['date'][i])  #stock['date'][i]為datetime.date物件
stock.index = indexlist  #索引值改成日期
stock = stock.drop(['date'],axis = 1)  #刪除日期欄位
mlist = []
for item in stock.index:   #建立月份串列
    mlist.append(item.month)
stock['month'] = mlist  #新增月份欄位
#print(stock)
result = stock
for item in result[result.close > 21]:  #收盤價大於21元
    print(item)
    
print(result[(result.index >= "2018-06-01") & (result.index <= "2018-06-30") & (result.close >= 21)])  #六月份大於21元

tmp = result.sort_values(by = 'close', ascending=False)   #依照收盤價排序
print(tmp[:3])  #取收盤價前三高

print(result.loc["2018-06-01":"2018-06-30"])  #只顯示2018六月份

print(result.loc["2018-01-01":"2018-12-31"].groupby('month').close.count())  #2018每個月幾個營業日

print(result.loc["2018-01-01":"2018-12-31"].groupby('month').shares.sum())  #2018每個月累計成交股數

result.loc["2018-01-01":"2018-12-31"].groupby('month').shares.sum().plot()  #2018月累計成交股數圖
pp.ylabel('shares')
pp.title('month of shares')

```

## MongoDB 備份

```sh
mongodump -h 127.0.0.1:27017 -d TwStock -o ./mongo-backup
```




