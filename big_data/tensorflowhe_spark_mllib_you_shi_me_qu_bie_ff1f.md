# TensorFlow和Spark MLlib有什麼區別？



Spark 是為一般的數據處理設計的，並不特定於機器學習。但是使用 MLlib for Spark，也可以在 Spark 上進行機器學習。在基本的設置中，Spark 將模型參數存儲在驅動器節點，工作器與驅動器通信從而在每次迭代後更新這些參數。對於大規模部署而言，這些模型參數可能並不適合驅動器，並且會作為一個 RDD 而進行維護更新。這會帶來大量額外開銷，因為每次迭代都需要創造一個新的 RDD 來保存更新後的模型參數。更新模型涉及到在整個機器/磁盤上重排數據，這就限制了 Spark 的擴展性。這是 Spark 的基本數據流模型(DAG)的不足之處。Spark 並不能很好地支持機器學習所需的迭代

谷歌有一個基於參數服務器模型的分佈式機器學習平臺 DistBelief。DistBelief 的主要缺陷是：為了編寫機器學習應用，需要操作低級代碼。谷歌想要自己的所有員工無需精通分佈式執行就能編寫機器學習代碼——基於同樣的理由，谷歌為大數據處理編寫了 MapReduce 框架。

所以為了實現這一目標，谷歌設計了 TensorFlow。TensorFlow 採用了數據流範式，但是是一種更高級的版本——其中計算圖無需是 DAG，而且包含循環且支持可變狀態。

TensorFlow 使用節點和邊的有向圖來表示計算。節點表示計算，狀態可變。而邊則表示多維數據數組(張量)，在節點之間傳輸。TensorFlow 需要用戶靜態聲明這種符號計算圖，並對該圖使用複寫和分區(rewrite & partitioning)將其分配到機器上進行分佈式執行。(MXNet，尤其是 DyNet 使用了圖的動態聲明，這改善了編程的難度和靈活性。)



Spark 在兩層神經網絡上有更大的性能損失。這是因為兩層網絡需要更多迭代計算。