# shmqueue
   基於c++內存池和共享內存高速的進程間通信隊列
# 實現原理
   把消息隊列對象生成在實現分配好的共享內存區中（共享內存去的大小遠遠大於消息隊列管理對象messagequeue），對象中記錄者共享內存去剩餘內存的數據情況，消息內存
區的管理方式基於緩行管理方式，詳細介紹：http://blog.csdn.net/suhuaiqiang_janlay/article/details/51194984
# 使用方式
   讀進程和寫進程的唯一區別就是main函數中註釋部分，一個調用send，一個調用get
# 注意事項
   目前一個messagequeue的數據傳輸方向只能是單向的，一個進程要麼讀，要麼寫，這裡並沒有進行額外的同步措施，也是為了提高數據傳輸的速度，如果改成雙向的要把
 數據的索引begin，end等改為atomic_int保證進程間的數據同步的同時還要保證隊列中數據段的同步，否則可能出現數據段的數據寫完還沒來得及更改索引的情況下，被另
 外一個進程幹擾，造成數據異常，也就是說要保證修改索引個寫數據兩個操作合起來是一個原子行為。
