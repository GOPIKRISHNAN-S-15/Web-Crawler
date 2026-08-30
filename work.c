#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <curl/curl.h>
#define MAX_URL_LENGTH 2048
#define MAX_PAGES 100
#define MAX_LINKS 500
typedef struct Node {
    char url[MAX_URL_LENGTH];
    struct Node *next;
} Node;

typedef struct {
    Node *front;
    Node *rear;
} Queue;

typedef struct {
    char *data;
    size_t size;
} WebPage;
void initQueue(Queue *q) {
    q->front = NULL;
    q->rear = NULL;
}
int isQueueEmpty(Queue *q) {
    return q->front == NULL;
}
void enqueue(Queue *q, const char *url) {
    Node *newNode = malloc(sizeof(Node));
    if (newNode == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    strncpy(newNode->url, url, MAX_URL_LENGTH - 1);
    newNode->url[MAX_URL_LENGTH - 1] = '\0';
    newNode->next = NULL;
    if (q->rear == NULL) {
        q->front = q->rear = newNode;
    } else {
        q->rear->next = newNode;
        q->rear = newNode;
    }
}
char *dequeue(Queue *q) {
    if (isQueueEmpty(q))
        return NULL;
    Node *temp = q->front;
    q->front = q->front->next;
    if (q->front == NULL)
        q->rear = NULL;
    char *url = malloc(MAX_URL_LENGTH);
    if (url == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    strcpy(url, temp->url);
    free(temp);
    return url;
}
size_t writeCallback(void *contents,
                     size_t size,
                     size_t nmemb,
                     void *userp) {
    size_t totalSize = size * nmemb;
    WebPage *page = (WebPage *)userp;
    char *newData = realloc(
        page->data,
        page->size + totalSize + 1
    );
    if (newData == NULL)
        return 0;
    page->data = newData;
    memcpy(
        &(page->data[page->size]),
        contents,
        totalSize
    );
    page->size += totalSize;
    page->data[page->size] = '\0';
    return totalSize;
}
int downloadPage(const char *url, WebPage *page) {
    CURL *curl;
    CURLcode result;
    page->data = NULL;
    page->size = 0;
    curl = curl_easy_init();
    if (curl == NULL)
        return 0;
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(
        curl,
        CURLOPT_WRITEFUNCTION,
        writeCallback
    );
    curl_easy_setopt(
        curl,
        CURLOPT_WRITEDATA,
        page
    );
    curl_easy_setopt(
        curl,
        CURLOPT_FOLLOWLOCATION,
        1L
    );
    curl_easy_setopt(
        curl,
        CURLOPT_TIMEOUT,
        10L
    );
    curl_easy_setopt(
        curl,
        CURLOPT_USERAGENT,
        "SimpleCWebCrawler/1.0"
    );
    result = curl_easy_perform(curl);
    if (result != CURLE_OK) {
        fprintf(
            stderr,
            "Download failed: %s\n",
            curl_easy_strerror(result)
        );
        curl_easy_cleanup(curl);
        free(page->data);
        page->data = NULL;
        return 0;
    }
    curl_easy_cleanup(curl);
    return 1;
}
int isVisited(
    char visited[][MAX_URL_LENGTH],
    int count,
    const char *url
) {
    for (int i = 0; i < count; i++) {
        if (strcmp(visited[i], url) == 0)
            return 1;
    }
    return 0;
}
int isValidURL(const char *url) {

    return (
        strncmp(url, "http://", 7) == 0 ||
        strncmp(url, "https://", 8) == 0
    );
}
void getDomain(
    const char *url,
    char *domain
) {
    const char *start;
    if (strncmp(url, "https://", 8) == 0)
        start = url + 8;
    else if (strncmp(url, "http://", 7) == 0)
        start = url + 7;
    else {
        domain[0] = '\0';
        return;
    }
    int i = 0;
    while (
        start[i] != '\0' &&
        start[i] != '/' &&
        start[i] != '?' &&
        start[i] != '#'
    ) {
        domain[i] = start[i];
        i++;
    }
    domain[i] = '\0';
}
void makeAbsoluteURL(
    const char *baseURL,
    const char *link,
    char *absoluteURL
) {
    if (
        strncmp(link, "http://", 7) == 0 ||
        strncmp(link, "https://", 8) == 0
    ) {
        strncpy(
            absoluteURL,
            link,
            MAX_URL_LENGTH - 1
        );
        absoluteURL[MAX_URL_LENGTH - 1] = '\0';
        return;
    }
    char domain[MAX_URL_LENGTH];
    getDomain(baseURL, domain);
    if (link[0] == '/') {
        char protocol[10];
        if (strncmp(baseURL, "https://", 8) == 0)
            strcpy(protocol, "https://");
        else
            strcpy(protocol, "http://");
        snprintf(
            absoluteURL,
            MAX_URL_LENGTH,
            "%s%s%s",
            protocol,
            domain,
            link
        );
        return;
    }
    absoluteURL[0] = '\0';
}
int extractLinks(
    const char *html,
    const char *baseURL,
    char links[][MAX_URL_LENGTH]
) {
    int count = 0;
    const char *position = html;
    while (
        (position = strstr(position, "href=")) != NULL &&
        count < MAX_LINKS
    ) {
        position += 5;
        while (*position == ' ' ||
               *position == '\t')
            position++;
        char quote = *position;
        if (quote != '"' && quote != '\'')
            continue;
        position++;
        const char *end = strchr(position, quote);
        if (end == NULL)
            break;
        size_t length = end - position;
        if (length == 0 ||
            length >= MAX_URL_LENGTH) {
            position = end + 1;
            continue;
        }
        char rawURL[MAX_URL_LENGTH];
        strncpy(
            rawURL,
            position,
            length
        );
        rawURL[length] = '\0';
        if (rawURL[0] == '#') {
            position = end + 1;
            continue;
        }
        if (
            strncmp(rawURL, "javascript:", 11) == 0 ||
            strncmp(rawURL, "mailto:", 7) == 0 ||
            strncmp(rawURL, "tel:", 4) == 0
        ) {
            position = end + 1;
            continue;
        }
        char absoluteURL[MAX_URL_LENGTH];
        makeAbsoluteURL(
            baseURL,
            rawURL,
            absoluteURL
        );
        if (isValidURL(absoluteURL)) {
            strncpy(
                links[count],
                absoluteURL,
                MAX_URL_LENGTH - 1
            );
            links[count][MAX_URL_LENGTH - 1] = '\0';
            count++;
        }
        position = end + 1;
    }
    return count;
}
void freeQueue(Queue *q) {
    while (!isQueueEmpty(q)) {
        char *url = dequeue(q);
        free(url);
    }
}
void crawl(
    const char *startURL,
    int maxPages
) {
    Queue queue;
    char visited[MAX_PAGES][MAX_URL_LENGTH];
    int visitedCount = 0;
    char rootDomain[MAX_URL_LENGTH];
    initQueue(&queue);
    getDomain(startURL, rootDomain);
    enqueue(&queue, startURL);
    printf("\n");
    printf("========================================\n");
    printf("        REAL-TIME C WEB CRAWLER\n");
    printf("========================================\n");
    printf("Starting URL : %s\n", startURL);
    printf("Domain       : %s\n", rootDomain);
    printf("Page limit   : %d\n", maxPages);
    printf("----------------------------------------\n\n");
    while (
        !isQueueEmpty(&queue) &&
        visitedCount < maxPages
    ) {
        char *currentURL = dequeue(&queue);
        if (currentURL == NULL)
            break;
        if (
            isVisited(
                visited,
                visitedCount,
                currentURL
            )
        ) {

            free(currentURL);
            continue;
        }
        strncpy(
            visited[visitedCount],
            currentURL,
            MAX_URL_LENGTH - 1
        );
        visited[visitedCount][MAX_URL_LENGTH - 1] = '\0';
        visitedCount++;
        printf(
            "[%d] Crawling: %s\n",
            visitedCount,
            currentURL
        );
        WebPage page;
        if (!downloadPage(currentURL, &page)) {
            printf("    -> Failed to download\n\n");
            free(currentURL);
            continue;
        }
        printf(
            "    -> Downloaded %zu bytes\n",
            page.size
        );
        char links[MAX_LINKS][MAX_URL_LENGTH];
        int linkCount =
            extractLinks(
                page.data,
                currentURL,
                links
            );
        printf(
            "    -> Found %d links\n",
            linkCount
        );
        int added = 0;
        for (int i = 0; i < linkCount; i++) {
            char linkDomain[MAX_URL_LENGTH];
            getDomain(
                links[i],
                linkDomain
            );
            if (
                strcmp(
                    rootDomain,
                    linkDomain
                ) != 0
            ) {
                continue;
            }
            if (
                !isVisited(
                    visited,
                    visitedCount,
                    links[i]
                )
            ) {

                enqueue(
                    &queue,
                    links[i]
                );
                added++;
            }
        }
        printf(
            "    -> Added %d new URLs to queue\n\n",
            added
        );
        free(page.data);
        free(currentURL);
    }
    printf("----------------------------------------\n");
    printf("CRAWLING COMPLETED\n");
    printf("----------------------------------------\n");
    printf(
        "Total pages crawled: %d\n\n",
        visitedCount
    );
    printf("Visited URLs:\n");
    for (int i = 0; i < visitedCount; i++) {

        printf(
            "%d. %s\n",
            i + 1,
            visited[i]
        );
    }
    freeQueue(&queue);
}
int main(void) {
    char startURL[MAX_URL_LENGTH];
    int maxPages;
    printf("Enter starting URL: ");
    scanf(
        "%2047s",
        startURL
    );
    printf("Maximum pages to crawl: ");
    scanf(
        "%d",
        &maxPages
    );
    if (maxPages <= 0 ||
        maxPages > MAX_PAGES) {
        printf(
            "Please enter a page limit between 1 and %d.\n",
            MAX_PAGES
        );
        return 1;
    }
    if (!isValidURL(startURL)) {
        printf(
            "Invalid URL. Use http:// or https://\n"
        );
        return 1;
    }
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != 0) {
        printf(
            "Failed to initialize libcurl.\n"
        );
        return 1;
    }
    crawl(
        startURL,
        maxPages
    );
    curl_global_cleanup();
    return 0;
}
