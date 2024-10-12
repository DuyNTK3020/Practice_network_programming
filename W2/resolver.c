#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>

// Định nghĩa API key cho VirusTotal
#define VIRUSTOTAL_API_KEY "17540209db22951450b301fb2fbbb1825aca35756f9eec4bdc881d289f527cc1"

// Cấu trúc để lưu trữ phản hồi từ CURL
struct CURLResponse
{
    char *html; // Nội dung HTML
    size_t size; // Kích thước của nội dung
};

// Hàm callback để ghi dữ liệu HTML từ CURL
size_t WriteHTMLCallback(void *content, size_t size, size_t nmemb, void *userp)
{
    size_t actualSize = size * nmemb;
    struct CURLResponse *response = (struct CURLResponse *)userp;
    char *ptr = realloc(response->html, response->size + actualSize + 1);

    if (ptr == NULL)
    {
        printf("Not enough memory available (realloc returned NULL)\n");
        return 0;
    }

    response->html = ptr;
    memcpy(&(response->html[response->size]), content, actualSize);
    response->size += actualSize;
    response->html[response->size] = 0;

    return actualSize;
}

// Hàm truy vấn VirusTotal để kiểm tra tên miền
void queryVirusTotal(const char *domain)
{
    CURL *curl;
    CURLcode result;
    struct CURLResponse response;

    response.html = malloc(1);
    response.size = 0;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl)
    {
        char requestUrl[256];
        snprintf(requestUrl, sizeof(requestUrl), "https://www.virustotal.com/vtapi/v2/domain/report?apikey=%s&domain=%s", VIRUSTOTAL_API_KEY, domain);

        curl_easy_setopt(curl, CURLOPT_URL, requestUrl);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteHTMLCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);

        result = curl_easy_perform(curl);
        if (result != CURLE_OK)
        {
            fprintf(stderr, "GET request failed: %s\n", curl_easy_strerror(result));
        }
        else
        {
            char *positiveCases = strstr(response.html, "\"positives\":");
            char *totalCases = strstr(response.html, "\"total\":");

            if (positiveCases != NULL && totalCases != NULL)
            {
                int positives = atoi(positiveCases + 12);
                int total = atoi(totalCases + 8);

                printf("Positives: %d / Total: %d\n", positives, total);

                if (positives > 0 && (double)positives / total > 0.2)
                {
                    printf("Domain is considered malicious.\n");
                }
                else
                {
                    printf("Domain is clean.\n");
                }
            }
            else
            {
                printf("Could not determine the status of the domain.\n");
            }
        }

        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    free(response.html);
}

// Hàm lấy địa chỉ IP từ tên miền
void getIpFromDomain(const char *domain)
{
    struct hostent *hostEntry;
    struct in_addr **addressList;

    if ((hostEntry = gethostbyname(domain)) == NULL)
    {
        printf("Not found information\n");
        return;
    }

    printf("Official IP: %s\n", inet_ntoa(*(struct in_addr *)hostEntry->h_addr));

    addressList = (struct in_addr **)hostEntry->h_addr_list;
    printf("Alias IP:\n");
    for (int i = 1; addressList[i] != NULL; i++)
    {
        printf("%s\n", inet_ntoa(*addressList[i]));
    }
}

// Hàm lấy tên miền từ địa chỉ IP
void getDomainFromIp(const char *ip)
{
    struct sockaddr_in sa;
    char hostname[1024];

    memset(&sa, 0, sizeof(struct sockaddr_in));
    sa.sin_family = AF_INET;

    if (getnameinfo((struct sockaddr *)&sa, sizeof sa, hostname, sizeof hostname, NULL, 0, 0) != 0)
    {
        printf("Not found information\n");
        return;
    }

    printf("Official name: %s\n", hostname);
    printf("Alias name: N/A\n");
}

// Hàm kiểm tra xem đầu vào có phải là địa chỉ IP không
int checkIfIp(const char *input)
{
    struct sockaddr_in sa;
    return inet_pton(AF_INET, input, &(sa.sin_addr)) != 0;
}

// Hàm thực hiện yêu cầu GET và trả về phản hồi
struct CURLResponse GetRequest(CURL *curl_handle, const char *url)
{
    CURLcode res;
    struct CURLResponse response;

    response.html = malloc(1);
    response.size = 0;

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteHTMLCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&response);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/117.0.0.0 Safari/537.36");
    res = curl_easy_perform(curl_handle);

    if (res != CURLE_OK)
    {
        fprintf(stderr, "GET request failed: %s\n", curl_easy_strerror(res));
    }

    return response;
}

// Hàm phân tích HTML và lưu trữ thông tin vào các tệp CSV
void parseHTML(const char *html, size_t size)
{
    htmlDocPtr doc = htmlReadMemory(html, (unsigned long)size, NULL, NULL, HTML_PARSE_NOERROR);
    xmlXPathContextPtr context = xmlXPathNewContext(doc);

    // Links
    xmlXPathObjectPtr linkHTMLElements = xmlXPathEvalExpression((xmlChar *)"//a", context);
    FILE *csvFileLink = fopen("links.csv", "w");
    fprintf(csvFileLink, "url,linkText\n");
    for (int i = 0; i < linkHTMLElements->nodesetval->nodeNr; ++i)
    {
        xmlNodePtr linkNode = linkHTMLElements->nodesetval->nodeTab[i];
        char *url = (char *)xmlGetProp(linkNode, (xmlChar *)"href");
        char *linkText = (char *)xmlNodeGetContent(linkNode);
        fprintf(csvFileLink, "%s,%s\n", url, linkText);

        free(url);
        free(linkText);
    }
    fclose(csvFileLink);
    xmlXPathFreeObject(linkHTMLElements);

    // Text
    xmlXPathObjectPtr textHTMLElements = xmlXPathEvalExpression((xmlChar *)"//p|//h1|//h2|//h3|//h4|//h5|//h6", context);
    FILE *csvFileText = fopen("texts.csv", "w");
    fprintf(csvFileText, "text\n");
    for (int i = 0; i < textHTMLElements->nodesetval->nodeNr; ++i)
    {
        xmlNodePtr textNode = textHTMLElements->nodesetval->nodeTab[i];
        char *text = (char *)xmlNodeGetContent(textNode);
        fprintf(csvFileText, "%s\n", text);
        free(text);
    }
    fclose(csvFileText);
    xmlXPathFreeObject(textHTMLElements);

    // Video
    xmlXPathObjectPtr videoHTMLElements = xmlXPathEvalExpression((xmlChar *)"//video|//iframe[contains(@src, 'youtube') or contains(@src, 'vimeo')]", context);
    FILE *csvFileVideo = fopen("videos.csv", "w");
    fprintf(csvFileVideo, "video\n");
    for (int i = 0; i < videoHTMLElements->nodesetval->nodeNr; ++i)
    {
        xmlNodePtr videoNode = videoHTMLElements->nodesetval->nodeTab[i];
        char *videoURL = (char *)xmlGetProp(videoNode, (xmlChar *)"src");
        fprintf(csvFileVideo, "videoURL\n");
        free(videoURL);
    }
    fclose(csvFileVideo);
    xmlXPathFreeObject(videoHTMLElements);

    xmlXPathFreeContext(context);
    xmlFreeDoc(doc);
    xmlCleanupParser();
}

// Hàm main
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <hostname or IP address>\n", argv[0]);
        return 1;
    }

    queryVirusTotal(argv[1]);

    curl_global_init(CURL_GLOBAL_ALL);

    CURL *curl_handle = curl_easy_init();

    char url[256];
    if (checkIfIp(argv[1]))
    {
        snprintf(url, sizeof(url), "https://%s/", argv[1]);
        getDomainFromIp(argv[1]);
    }
    else
    {
        snprintf(url, sizeof(url), "https://%s/", argv[1]);
        getIpFromDomain(argv[1]);
    }

    struct CURLResponse response = GetRequest(curl_handle, url);

    parseHTML(response.html, response.size);

    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();
    free(response.html);

    return 0;
}