## Interleaving String

Given s1, s2, s3, find whether s3 is formed by the interleaving of s1 and s2.

For example,
Given:
```
s1 = "aabcc",
s2 = "dbbca",

When s3 = "aadbbcbcac", return true.
When s3 = "aadbbbaccc", return false.
```

## Solution

遞歸法

設三個字符指針分別為`s1,s2,s3`, 當前位置`i, j , k`, 判決函數`isInterleave(i, j, k)`:

* `s1,s2,s3`都為空，返回`true`
* len表示字符串長度，若`len(s1) + len(s2) != len(s3)`，返回`false`
* 若`s1[i] == s3[k],DFS搜尋，`isInterleave(i + 1, j, k + 1) == true`, 返回true， 否則下一步
* 若`s2[j] == s3[k], DFS搜尋, `isInterleave(i, j + 1, k + 1) == true`, 返回true， 否則下一步
* 返回false

## Code
```cpp
bool isInterleave(const char *s1, const char *s2, const char *s3) {
	if (*s1 == 0 && *s2 == 0 && *s3 == 0)
		return true;
	if (*s1 == *s3 && isInterleave(s1 + 1, s2, s3 + 1))
		return true;
	if (*s2 == *s3 && isInterleave(s1, s2 + 1, s3 + 1))
		return true;
	return false;
}
```

代碼簡單，思路簡單，純粹就是DFS。缺點是當有大量字符相等時，搜索路徑很大，深度高，因此TLE

## Solution 2

DP法

設`dp[i][j]`,表示s1前i個字符，s2前j個字符匹配s3 前i + j個字符情況.

* 若`s3[i + j - 1] == s1[i - 1] && dp[i - 1][j] == true`, `dp[i][j] = true`,否則下一步:
* 若`s3[i + j - 1] == s2[j - 1] && dp[i][j - 1] == true`, `dp[i][j] = true`，否則下一步
* `dp[i][j] = false`

## Code
```cpp
bool dp_isInterleave(string s1, string s2, string s3) {
	int len1 = s1.size(), len2 = s2.size(), len3 = s3.size();
	if (len1 + len2 != len3)
		return false;
	vector<vector<int>> dp(len1 + 1, vector<int>(len2 + 1, false));
	dp[0][0] = true;// 空字符匹配空字符
	for (int i = 1; i <= len1; ++i) // 只匹配s1
		if (s1[i - 1] == s3[i - 1])
			dp[i][0] = true;
		else
			break;
	for (int j = 1; j <= len2; ++j) // 只匹配s2
		if (s2[j - 1] == s3[j - 1])
			dp[0][j] = true;
		else
			break;
	for (int i = 1; i <= len1; ++i)
		for (int j = 1; j <= len2; ++j) {
			if (s1[i - 1] == s3[i + j - 1])
				dp[i][j] |= dp[i-1][j];
			if (s2[j - 1] == s3[i + j - 1])
				dp[i][j] |= dp[i][j - 1];
		}
	return dp[len1][len2];
}
```

## cpp 重載問題

```cpp
void foo(string s) {
	foo(s.c_str());
}
void foo(char *s) {
	cout << s << endl;
}
```
以上代碼會暴棧，原因在於`foo(string s)`不會調用`foo(char *s)`,而是調用其自身。。。。

一個原因是`s.c_str()`返回的是`const char *`和`char *`簽名不一樣，因此不調用。。。。。
```cpp
void foo(string s) {
	foo(s.c_str());
}
void foo(const char *s) {
	cout << s << endl;
}
```

以上代碼仍然不能正常運行，原因是`foo(string s)`聲明時`foo(const char *s)`並沒有聲明，找不到該函數，因此和自身匹配調用自身.

```cpp
void foo(const char *s)
{
	cout << s << endl;
}
void foo(string s)
{
	foo(s.c_str());
}
```

以上代碼運行正常。

**總結： c++ 是一門複雜危險的語言**
