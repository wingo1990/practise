#include <stdio.h>



void ajustHeap(int arr[], int i, int length)
{
	int tmp = arr[i];
	for (int k = 2*i+1; k < length; k=2*k+1) {
		if (k+1 < length && arr[k] > arr[k+1])
			k++;

		if (tmp > arr[k]) {
			arr[i] = arr[k];
			i = k;
		} else {
			break;
		}
	
		arr[k] = tmp;
	}
}


void swap(int *a, int *b) 
{
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

void heapSort(int arr[], int length)
{
	for (int i = length/2 - 1; i >= 0; --i) {
		ajustHeap(arr, i, length);
	}

	while (length > 1) {
		swap(&arr[0], &arr[length - 1]);
		length = length - 1;
		ajustHeap(arr, 0, length);
	}
}

/////////////////////////////////////////////////////////////////////////////////

int partition(int arr[], int start, int end)
{
	int pivot = arr[start];
	int index = start;
	while (start < end) {
		while(start < end && arr[end] >= pivot)
			end--;
		while(start < end && arr[start] <= pivot)
			start++;
		
		if (start < end)
			swap(&arr[start], &arr[end]);
	}
	
	swap(&arr[index], &arr[start]);
	return start;
}

void qsort_s(int arr[], int start, int end)
{
	if (start >= end)
		return;

	int mid = partition(arr, start, end);

	qsort_s(arr, start, mid-1);
	qsort_s(arr, mid+1, end);
}

void Qsort(int arr[], int length)
{
	qsort_s(arr, 0, length - 1);
}

int main()
{
	int arr[] = {1, 1, 1, 1, 2, 0, 4, 4, 5, 7};
	int arr1[] = {1,2,7,7,7,7,7,8,9,10};
	int arr2[] = {10, 9,8,7,6,5,4,3,2,1,0};

	int arrlen = sizeof(arr)/sizeof(arr[0]);
	//heapSort(arr, arrlen);
	Qsort(arr, arrlen);
	for (int i = 0; i < arrlen; ++i) {
		printf("%d ", arr[i]);
	}
	printf("\n");

	int arr1len = sizeof(arr1)/sizeof(arr1[0]);
	//heapSort(arr1, arr1len);
	Qsort(arr1, arr1len);
	for (int i = 0; i < arr1len; ++i) {
		printf("%d ", arr1[i]);
	}
	printf("\n");

	int arr2len = sizeof(arr2)/sizeof(arr2[0]);
	//heapSort(arr2, arr2len);
	Qsort(arr2, arr2len);
	for (int i = 0; i < arr2len; ++i) {
		printf("%d ", arr2[i]);
	}
	printf("\n");
}
