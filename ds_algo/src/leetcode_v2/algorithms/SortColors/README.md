## Sort Colors 

Given an array with n objects colored red, white or blue, sort them so that objects of the same color are adjacent, with the colors in the order red, white and blue.

Here, we will use the integers 0, 1, and 2 to represent the color red, white, and blue respectively.

Note:
You are not suppose to use the library's sort function for this problem.


Follow up:
A rather straight forward solution is a two-pass algorithm using counting sort.
First, iterate the array counting number of 0's, 1's, and 2's, then overwrite array with total number of 0's, then 1's and followed by 2's.

Could you come up with an one-pass algorithm using only constant space?

## Solution

由於數字在一定範圍以內，首先排除常規的排序算法。

顯然可以使用計數排序，分別統計0,1,2的次數，然後逐一拷貝即可,時間複雜度O(n)，空間複雜度O(1)


但以上方法，顯然需要2趟，能不能1趟就搞定呢？


顯然如果所有的0和2都排好了，那1自然也排好了。我們使用兩個指針zero, two分別指向0,2的位置，當掃描數組a[i]等於0時，交換a[i]和a[zero]位置，
同時更新zero指針。同理當a[i] == 2時，交換a[two]和a[i],並更新two。

## Code
```c
void sortColors(int a[], int n)
{
	if (a == NULL || n == 1)
		return;
	int zero = 0, two = n - 1;
	for (int i = 0; i <= two; ++i) {
		if (a[i] == 0) {
			swap(&a[i], &a[zero++]);
		}
		if (a[i] == 2) {
			swap(&a[i--], &a[two--]);
		}
	}
}
```
