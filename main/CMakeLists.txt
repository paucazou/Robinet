# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS "main.c ../esp_cron/cron.c ../esp_cron/library/jobs/jobs.c ../esp_cron/library/ccronexpr/ccronexpr.c ../esp_cron/include/timegm.c wifi.c")
set(COMPONENT_ADD_INCLUDEDIRS "../esp_cron ../esp_cron/include ../esp_cron/library/ccronexpr ../esp_cron/library/jobs")


register_component()
