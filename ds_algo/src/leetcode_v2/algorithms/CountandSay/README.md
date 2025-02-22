## Count and Say

The count-and-say sequence is the sequence of integers beginning as follows:
`1, 11, 21, 1211, 111221, ...`

`1` is read off as `"one 1"` or `11`.
`11` is read off as `"two 1s"` or `21`.
`21` is read off as `"one 2, then one 1"` or `1211`.

Given an integer `n`, generate the nth sequence.

Note: The sequence of integers will be represented as a string. 

## Solution
題目就是從原來的字符串進行字符統計組合成一個新的字符串，

比如`aaabcc`:

* 有3個a => 3a
* 有1一個b => 1b
* 有2個c => 2c

因此最後的結果為`3a1b2c`

直接一一迭代。
```cpp
class Solution {
	public:
		string countAndSay(int n) {
			n--;
			string result = "1";
			while (n--) {
				result = next(result);
			}
			return result;
		}
	private:
		string next(string s) {
			int n = s.size();
			if (n < 1)
				return "";
			char cur = s[0];
			int count = 1;
			string result;
			for (int i = 1; i < n; ++i) {
				if (cur != s[i]) {
					result += itos(count);
					result.push_back(cur);
					cur = s[i];
					count = 1;
				} else {
					++count;
				}
			}
			result += itos(count);
			result += cur;
			return result;
		}
		string itos(int i) {
			ss.str("");
			ss.clear();
			ss << i;
			return ss.str();
		}
		stringstream ss;
};
```

## int轉化成字符串方法

注意不能使用字符串相加得到，這習慣java的特別注意，即不能這樣`"" + 123`, 而且字符串也不能直接`push_back`數字, 會把數字轉成`char`對應的acii碼

### 1.使用sprintf函數, 需要自己開闢空間，注意類型轉化和格式化

```cpp
char buf[20];
sprintf(buf, "%d", 123);
string s1(buf);
cout << "s1 = " << s1 << endl;
```
### 2.使用stringstream, 推薦使用，內置buffer，自動識別類型

```cpp
stringstream ss;
ss << 123;
string s2 = ss.str();
cout << "s2 = " << s2 << endl;
```

## string轉化成int

### 1.使用stoi

```cpp
string s1 = "123";
cout << stoi(s1) << endl;
```
### 2.使用sscanf

```cpp
int i;
sscanf(s1.c_str(), "%d", &i);
cout << i << endl;
```

### 3.使用stringstream
```cpp
int j;
ss << s1;
ss >> j;
cout << j << endl;
```


## 證明count and say出現的數字不會超過4

從discuss引用的

>Proof by exhaustion and contrapositive:
>
>In order for a number greater than 4 to be created, there must be a series of numbers n>4 in length all the same digit. Therefore, there is a subset of that series where the count would only reach 4. Because of this, any proof for the existence of a chain resulting in a number greater than 4 is also a proof for the existence of a 4-chain. Using the proof by contrapositive, this means that if 4-chains are proved to be impossible, then any n-chain with n>4 is also impossible.
>
>In order to start with a chain with numbers greater than 4, you must assume that a 4-chain is possible in the first place, which is circular reasoning, and so cannot be used as an initial point. It is further impossible to have a negative value, since the counting numbers do not include them.
>
>Therefore, the only chains able to create a 4 (at least the first one) are 0000, 1111, 2222, or 3333.
>
>0 0 0 0 -> 40
>
>The 0000 is read zero 0, zero 0, which must come from . Since there is nothing present, it could in theory occur anywhere in the string. However, since they would be next to each other, if the 0 is repeated as would be neccessary, the zeros would add together, resulting in just zero 0, leaving only 20, not 40.
>
>1 1 1 1 -> 41
>
>The 1111 is read one 1, one 1 (or 11), which translates to 21, not 1111. This contradicts the assumption that there is a way to get 1111, and so prevents 4 or greater from appearing. Therefore, 1s cannot reach 4.
>
>2 2 2 2 -> 42
>
>The 2222 is read two 2, two 2 (or 22 22), which is identical to the output. Since the input maps to itself, there is no way to leave that cycle, or it already would have. If 2222 exists in the input, then 2222 must have mapped to it. It cannot reach 42. Therefore, 2s cannot reach 4.
>
>3 3 3 3 -> 43
>
>The 3333 is read three 3, three 3 (or 333 333). This in turn would require 333 333 333. This fails in two respects. First, that the previous inputs would not merge to 63 or 93. The second, that the sequence eventually traces back to the origin, 1. Since it keeps increasing in length as the number of rounds since the start decreases, it cannot have started at 1. Therefore, 3s cannot reach 4.
>
>As every possible case has been examined, and none can reach a 4 while starting at the given beginning (1), it is not possible for a 4-chain to occur, meaning a 4 cannot appear in any valid string for this problem. Further, as stated above, since a 4-chain is impossible, so too are all n-chains with n>4, so no number greater than 4 can appear either.

證明是使用了窮舉法和換置位法

要產生4，前面必須有4個連續的數字，比如41,反推前一個數字應該是1111, 然後繼續推前面的一個應該是11，但由11推出了是21，和1111矛盾

同理要有42->2222，2222的前面是2222，當2222的後面是42不是2222，循環矛盾。

43->3333, 3333的前面是333333，而這個會產生63而不是3333,矛盾

不能產生4，自然不能產生大於4的
