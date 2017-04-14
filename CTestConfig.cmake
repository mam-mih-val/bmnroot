# Dashboard is opened for submissions for a 24 hour period starting at
# the specified NIGHLY_START_TIME. Time is specified in 24 hour format.
set(CTEST_PROJECT_NAME "BmnRoot")
set(CTEST_NIGHTLY_START_TIME "01:00:00 UTC")

set(CTEST_DROP_METHOD "http")
set(CTEST_DROP_SITE "mpd.jinr.ru")
set(CTEST_DROP_LOCATION "/cdash/submit.php?project=BmnRoot")
set(CTEST_DROP_SITE_CDASH TRUE)

#set(CTEST_TESTING_TIMEOUT 300)
