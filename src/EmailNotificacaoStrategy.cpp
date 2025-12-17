#include "EmailNotificacaoStrategy.hpp"
#include "Logger.hpp"
#include <cstring>
#include <curl/curl.h>
#include <iostream>
#include <vector>

// Helper functions for Libcurl
namespace {
struct UploadStatus {
  const char *data;
  size_t size;
  size_t bytes_read;
};

static size_t payload_read_function(void *ptr, size_t size, size_t nmemb,
                                    void *userp) {
  struct UploadStatus *upload_ctx = (struct UploadStatus *)userp;

  if ((size * nmemb) < 1)
    return 0;

  if (upload_ctx->size > 0) {
    size_t copy_len =
        (upload_ctx->size > size * nmemb) ? size * nmemb : upload_ctx->size;
    memcpy(ptr, upload_ctx->data + upload_ctx->bytes_read, copy_len);
    upload_ctx->size -= copy_len;
    upload_ctx->bytes_read += copy_len;
    return copy_len;
  }
  return 0;
}
} // namespace

void EmailNotificacaoStrategy::enviar(const std::string &destinatario,
                                      const std::string &mensagem) {

  return;
  CURL *curl;
  CURLcode res = CURLE_OK;
  struct curl_slist *recipients = NULL;
  struct UploadStatus upload_ctx;

  // Email Config
  const std::string FROM = "ayrtonsilvamatos@gmail.com";
  // NOTE: In a real environment, use environment variables or a secure
  // configuration loader
  const std::string PASSWORD = "ahfm nbgq lmnt fqkc";

  // Construct Payload (Headers + Body)
  std::string payload_text = "To: " + destinatario + "\r\n" + "From: " + FROM +
                             "\r\n" +
                             "Subject: [MONITORAMENTO] Alerta de Consumo\r\n" +
                             "\r\n" + mensagem + "\r\n";

  upload_ctx.data = payload_text.c_str();
  upload_ctx.size = payload_text.size();
  upload_ctx.bytes_read = 0;

  curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_USERNAME, FROM.c_str());
    curl_easy_setopt(curl, CURLOPT_PASSWORD, PASSWORD.c_str());
    curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.gmail.com:465");
    curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);

    // Setup Recipients
    recipients = curl_slist_append(recipients, destinatario.c_str());
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

    // Setup payload callback
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_read_function);
    curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

    // Verbose for debg
    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    // Silencing "Conectando..." to avoid CLI clutter (User Request)
    // std::cout << "[EMAIL] Conectando ao SMTP Gmail para envio para: "
    //           << destinatario << "..." << std::endl;
    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
      // Logging error to DB instead of just stderr (or both)
      try {
        Logger::getInstance().logSystemError(
            LogLevel::ERROR, "EmailStrategy",
            "Falha envio curl: " + std::string(curl_easy_strerror(res)));
      } catch (...) {
        std::cerr
            << "[EMAIL] Erro FATAL: Logger nao inicializado. Curl failed: "
            << curl_easy_strerror(res) << std::endl;
      }
    } else {
      // Silencing success output to avoid CLI clutter as requested
      // std::cout << "[EMAIL] Enviado com sucesso!" << std::endl;
    }

    curl_slist_free_all(recipients);
    curl_easy_cleanup(curl);
  }
}
