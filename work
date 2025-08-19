#include <stdio.h> #include <stdlib.h> #include <string.h>

// Define the maximum URL length #define MAX_URL_LENGTH 100 #define MAX_URLS 10

// Node for Linked List typedef struct ListNode { char url[MAX_URL_LENGTH]; struct ListNode* next; } ListNode;

// Function to create a new list node ListNode* createListNode(const char* url) { ListNode* newNode = (ListNode*)malloc(sizeof(ListNode)); strcpy(newNode->url, url); newNode->next = NULL; return newNode; }

// Function to add a node to the end of the list void addListNode(ListNode** head, const char* url) { ListNode* newNode = createListNode(url); if (*head == NULL) { head = newNode; } else { ListNode temp = *head; while (temp->next != NULL) { temp = temp->next; } temp->next = newNode; } }

// Function to print the list void printList(ListNode* head, int depth) { ListNode* temp = head; while (temp != NULL) { for (int i = 0; i < depth; ++i) { printf(" "); } printf("%s\n", temp->url); temp = temp->next; } }

// Function to simulate URL fetching (stub function) void fetchURLs(const char* url, char urls[][MAX_URL_LENGTH], int* urlCount) { *urlCount = 0;

if (strcmp(url, "http://example.com") == 0) {
    strcpy(urls[(*urlCount)++], "http://example.com/about");
    strcpy(urls[(*urlCount)++], "http://example.com/contact");
} else if (strcmp(url, "http://example.com/about") == 0) {
    strcpy(urls[(*urlCount)++], "http://example.com/about/team");
    strcpy(urls[(*urlCount)++], "http://example.com/about/history");
} else if (strcmp(url, "http://example.com/contact") == 0) {
    strcpy(urls[(*urlCount)++], "http://example.com/contact/form");
    strcpy(urls[(*urlCount)++], "http://example.com/contact/map");
} else {
    *urlCount = 0;
}
}

// Main function to perform web crawling void crawl(const char* startURL) { ListNode* crawledList = NULL; ListNode* queue = createListNode(startURL);

while (queue != NULL) {
    ListNode* currentNode = queue;
    queue = queue->next;

    char* currentURL = currentNode->url;
    addListNode(&crawledList, currentURL);

    int urlCount;
    char urls[MAX_URLS][MAX_URL_LENGTH];
    fetchURLs(currentURL, urls, &urlCount);

    for (int i = 0; i < urlCount; ++i) {
        addListNode(&queue, urls[i]);
    }

    free(currentNode);
}

printList(crawledList, 0);
}

// Driver code int main() { crawl("http://example.com"); return 0; }
