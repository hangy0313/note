# (二、互斥對象和鎖）


##互斥(Mutex: Mutual Exclusion)

下面的代碼中兩個線程連續的往int_set中插入多個隨機產生的整數。

```cpp
std::set<int> int_set;
auto f = [&int_set]()
{
    try {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, 1000);

        for (std::size_t i = 0; i != 100000; ++i) {
            int_set.insert(dis(gen));
        }
    } catch (...) {}
};
std::thread td1(f), td2(f);
td1.join();
td2.join();
```

由於std::set::insert不是多線程安全的，多個線程同時對同一個對象調用insert其行為是未定義的(通常導致的結果是程序崩潰)。因此需要一種機制在此處對多個線程進行同步，保證任一時刻至多有一個線程在調用insert函數。

C++11提供了4個互斥對象(C++14提供了1個)用於同步多個線程對共享資源的訪問。

<table>
<thead>
<tr>
<th>類名</th>
<th>描述</th>
</tr>
</thead>
<tbody>
<tr>
<td>std::mutex</td>
<td>最簡單的互斥對象。</td>
</tr>
<tr>
<td>std::timed_mutex</td>
<td>帶有超時機制的互斥對象，允許等待一段時間或直到某個時間點仍未能獲得互斥對象的所有權時放棄等待。</td>
</tr>
<tr>
<td>std::recursive_mutex</td>
<td>允許被同一個線程遞歸的Lock和Unlock。</td>
</tr>
<tr>
<td>std::recursive_timed_mutex</td>
<td>顧名思義(bù jiě shì)。</td>
</tr>
<tr>
<td>std::shared_timed_mutex(C++14)</td>
<td>允許多個線程共享所有權的互斥對象，如讀寫鎖，本文不討論這種互斥。</td>
</tr>
</tbody>
</table>

## 鎖(Lock)

這裡的鎖是動詞而非名詞，互斥對象的主要操作有兩個加鎖(lock)和釋放鎖(unlock)。當一個線程對互斥對象進行lock操作並成功獲得這個互斥對象的所有權，在此線程對此對象unlock前，其他線程對這個互斥對象的lock操作都會被阻塞。

修改前面的代碼在兩個線程中對共享資源int_set執行insert操作前先對互斥對象mt進行加鎖操作，待操作完成後再釋放鎖。這樣就能保證同一時刻至多隻有一個線程對int_set對象執行insert操作。


```cpp
std::set<int> int_set;
std::mutex mt;
auto f = [&int_set, &mt]()
{
    try {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, 1000);

        for (std::size_t i = 0; i != 100000; ++i) {
            mt.lock();
            int_set.insert(dis(gen));
            mt.unlock();
        }
    } catch (...) {}
};
std::thread td1(f), td2(f);
td1.join();
td2.join();
```

## 使用`RAII`管理互斥對象

使用鎖時應避免發生死鎖(Deadlock)。`前面的代碼倘若一個線程在執行第10行的int_set.insert時拋出了異常，會導致第11行的unlock不被執行，從而可能導致另一個線程永遠的阻塞在第9行的lock操作`。類似的情況還有比如你寫了一個函數，`在進入函數後首先做的事情就是對某互斥對象執行lock操作，然而這個函數有許多的分支，並且其中有幾個分支要提前返回。因此你不得不在每個要提前返回的分支在返回前對這個互斥對象執行unlock操作。一但有某個分支在返回前忘了對這個互斥對象執行unlock，就可能會導致程序死鎖。`

為避免這類死鎖的發生，其他高級語言如C#提供了lock關鍵字、Java提供了synchronized關鍵字，它們都是通過finally關鍵字來實現的。比如對於C#

```cpp
lock(x)
{
    // do something
}
```
等價於


```java
System.Object obj = (System.Object)x;
System.Threading.Monitor.Enter(obj);
try
{
    // do something
} finally {
    System.Threading.Monitor.Exit(obj);
}
```


然而`C++並沒有try-finally`，事實上C++並不需要finally。C++通常使用`RAII(Resource Acquisition Is Initialization)來自動管理資源`。如果可能應總是使用標準庫提供的互斥對象管理類模板。

<table>
<thead>
<tr>
<th>類模板</th>
<th>描述</th>
</tr>
</thead>
<tbody>
<tr>
<td>std::lock_guard</td>
<td>嚴格基於作用域(scope-based)的鎖管理類模板，構造時是否加鎖是可選的(不加鎖時假定當前線程已經獲得鎖的所有權)，析構時自動釋放鎖，所有權不可轉移，對象生存期內不允許手動加鎖和釋放鎖。</td>
</tr>
<tr>
<td>std::unique_lock</td>
<td>更加靈活的鎖管理類模板，構造時是否加鎖是可選的，在對象析構時如果持有鎖會自動釋放鎖，所有權可以轉移。對象生命期內允許手動加鎖和釋放鎖。</td>
</tr>
<tr>
<td>std::shared_lock(C++14)</td>
<td>用於管理可轉移和共享所有權的互斥對象。</td>
</tr>
</tbody>
</table>

使用std::lock_guard類模板修改前面的代碼，在lck對象構造時加鎖，析構時自動釋放鎖，即使insert拋出了異常lck對象也會被正確的析構，所以也就不會發生互斥對象沒有釋放鎖而導致死鎖的問題。

```cpp
std::set<int> int_set;
std::mutex mt;
auto f = [&int_set, &mt]()
{
    try {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, 1000);

        for (std::size_t i = 0; i != 100000; ++i) {
            std::lock_guard<std::mutex> lck(mt);
            int_set.insert(dis(gen));
        }
    } catch (...) {}
};
std::thread td1(f), td2(f);
td1.join();
td2.join();
```
`互斥對象管理類模板`的加鎖策略

前面提到std::lock_guard、std::unique_lock和std::shared_lock類模板在構造時是否加鎖是可選的，C++11提供了`3種`加鎖策略。


<table>
<thead>
<tr>
<th>策略</th>
<th>tag type</th>
<th>描述</th>
</tr>
</thead>
<tbody>
<tr>
<td>(默認)</td>
<td>無</td>
<td>請求鎖，阻塞當前線程直到成功獲得鎖。</td>
</tr>
<tr>
<td>std::defer_lock</td>
<td>std::defer_lock_t</td>
<td>不請求鎖。</td>
</tr>
<tr>
<td>std::try_to_lock</td>
<td>std::try_to_lock_t</td>
<td>嘗試請求鎖，但不阻塞線程，鎖不可用時也會立即返回。</td>
</tr>
<tr>
<td>std::adopt_lock</td>
<td>std::adopt_lock_t</td>
<td>假定當前線程已經獲得互斥對象的所有權，所以不再請求鎖。</td>
</tr>
</tbody>
</table>

下表列出了互斥對象管理類模板對各策略的支持情況。

<table>
<thead>
<tr>
<th>策略</th>
<th>std::lock_guard</th>
<th>std::unique_lock</th>
<th>std::shared_lock</th>
</tr>
</thead>
<tbody>
<tr>
<td>(默認)</td>
<td>√</td>
<td>√</td>
<td>√(共享)</td>
</tr>
<tr>
<td>std::defer_lock</td>
<td>×</td>
<td>√</td>
<td>√</td>
</tr>
<tr>
<td>std::try_to_lock</td>
<td>×</td>
<td>√</td>
<td>√</td>
</tr>
<tr>
<td>std::adopt_lock</td>
<td>√</td>
<td>√</td>
<td>√</td>
</tr>
</tbody>
</table>

下面的代碼中std::unique_lock指定了std::defer_lock。

```cpp
std::mutex mt;
std::unique_lock<std::mutex> lck(mt, std::defer_lock);
assert(lck.owns_lock() == false);
lck.lock();
assert(lck.owns_lock() == true);
```

###對多個互斥對象加鎖
在某些情況下我們可能需要對多個互斥對象進行加鎖，考慮下面的代碼

```cpp
std::mutex mt1, mt2;
// thread 1
{
    std::lock_guard<std::mutex> lck1(mt1);
    std::lock_guard<std::mutex> lck2(mt2);
    // do something
}
// thread 2
{
    std::lock_guard<std::mutex> lck2(mt2);
    std::lock_guard<std::mutex> lck1(mt1);
    // do something
}
```

如果線程1執行到第5行的時候恰好線程2執行到第11行。那麼就會出現

- 線程1持有mt1並等待mt2
- 線程2持有mt2並等待mt1

發生死鎖。
為了避免發生這類死鎖，對於任意兩個互斥對象，在多個線程中進行加鎖時應保證其先後順序是一致。前面的代碼應修改成


```cpp
std::mutex mt1, mt2;
// thread 1
{
    std::lock_guard<std::mutex> lck1(mt1);
    std::lock_guard<std::mutex> lck2(mt2);
    // do something
}
// thread 2
{
    std::lock_guard<std::mutex> lck1(mt1);
    std::lock_guard<std::mutex> lck2(mt2);
    // do something
}
```

更好的做法是使用標準庫中的std::lock和std::try_lock函數來對多個Lockable對象加鎖。std::lock(或std::try_lock)會使用一種避免死鎖的算法對多個待加鎖對象進行lock操作(std::try_lock進行try_lock操作)，當待加鎖的對象中有不可用對象時std::lock會阻塞當前線程知道所有對象都可用(std::try_lock不會阻塞線程當有對象不可用時會釋放已經加鎖的其他對象並立即返回)。使用std::lock改寫前面的代碼，這裡刻意讓第6行和第13行的參數順序不同


```cpp
std::mutex mt1, mt2;
// thread 1
{
    std::unique_lock<std::mutex> lck1(mt1, std::defer_lock);
    std::unique_lock<std::mutex> lck2(mt2, std::defer_lock);
    std::lock(lck1, lck2);
    // do something
}
// thread 2
{
    std::unique_lock<std::mutex> lck1(mt1, std::defer_lock);
    std::unique_lock<std::mutex> lck2(mt2, std::defer_lock);
    std::lock(lck2, lck1);
    // do something
}
```

此外std::lock和std::try_lock還是異常安全的函數(要求待加鎖的對象unlock操作不允許拋出異常)，當對多個對象加鎖時，其中如果有某個對象在lock或try_lock時拋出異常，std::lock或std::try_lock會捕獲這個異常並將之前已經加鎖的對象逐個執行unlock操作，然後重新拋出這個異常(異常中立)。

並且std::lock_guard的構造函數lock_guard(mutex_type& m, std::adopt_lock_t t)也不會拋出異常。所以std::lock像下面這麼用也是正確

```cpp
std::lock(mt1, mt2);
std::lock_guard<std::mutex> lck1(mt1, std::adopt_lock);
std::lock_guard<std::mutex> lck2(mt2, std::adopt_lock);
```

