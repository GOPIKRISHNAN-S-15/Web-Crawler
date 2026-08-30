# Real-Time Web Crawler in C

A simple real-time web crawler written in **C** using **libcurl**. It downloads webpages, extracts hyperlinks, and crawls discovered pages using **Breadth-First Search (BFS)**.

## Features

* 🌐 Fetches real webpages using libcurl
* 🔗 Extracts links from HTML
* 🔄 Crawls pages using BFS
* ✅ Avoids duplicate URLs
* 🏠 Crawls within the starting domain
* 📄 Configurable page limit
* ⚠️ Handles download failures and timeouts

## Technologies

* C
* libcurl
* HTML
* BFS
* Queue / Linked List

## How to Run

Install dependencies on Ubuntu/WSL:

```bash
sudo apt update
sudo apt install gcc libcurl4-openssl-dev
```

Compile:

```bash
gcc crawler.c -o crawler -lcurl
```

Run:

```bash
./crawler
```

Enter a starting URL and the number of pages to crawl.

Example:

```text
Enter starting URL: https://example.com
Maximum pages to crawl: 5
```

## Workflow

```text
URL
 ↓
Download HTML
 ↓
Extract Links
 ↓
Check Visited URLs
 ↓
Add to Queue
 ↓
Crawl Next Page
```

## Project Status

**Current:** Real-time single-domain web crawling with BFS and hyperlink extraction.

**Future:** Better HTML parsing, URL normalization, robots.txt support, database storage, and web interface.

