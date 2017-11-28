
#include <prometheus/metric_registry.h>

#include <httplib.h>

#include <sstream>
#include <iostream>
#include <thread>

using namespace prometheus;
using namespace httplib;
using namespace std;
using namespace std::chrono;

atomic_bool running{true};

void shutdown_handler(int) {
  running = false;
}

string prometheus_to_string(const metric_registry &reg) {
  stringstream payload;
  reg.write(payload);
  return payload.str();
}

int main() {


  if (signal(SIGTERM | SIGKILL, shutdown_handler) == SIG_ERR) {
    return EXIT_FAILURE;
  }

  Server svr;
  metric_registry reg;

  cout << "Starting metric client..." << std::endl;

  auto &rpc_duration_family = reg.make_summary(metric_family::builder("rpc_duration", "Duration of the request"));
  auto &rpc_duration = rpc_duration_family.add({}, summary::objectives{
      {.01, .001}, //  1%
      {.05, .001}, //  5%
      {.5, .001},  // 50%
      {.90, .001}, // 90%
      {.99, .001}, // 99%
  });

  auto &some_histo_family = reg.make_histogram(metric_family::builder("request_time", "How long the response took."));
  auto &some_histo = some_histo_family.add({}, linear_bounds(1, 1, 10));

  auto &request_family = reg.make_counter(metric_family::builder("total_requests", "Total number of requests"));
  auto &request_counter = request_family.add({{"somekey", "somevalue"}});

  thread worker([&]() {
    while (running) {
      this_thread::sleep_for(milliseconds(1));
      request_counter.inc();
    }
    svr.stop();
  });

  svr.get("/metrics", [&](const Request &req, Response &res) {

    // time how long this request takes to serve
    rpc_duration.observe_duration([&]() {
      request_counter.inc();

      // some random data
      some_histo.observe(rand() % 10);

      res.set_content(prometheus_to_string(reg), "text/plain");
    });

  });

  cout << "Starting http server..." << std::endl;
  svr.listen(nullptr, 9999, AI_PASSIVE);
  running = false;

  if (worker.joinable()) {
    worker.join();
  }

  cout << "Shutting down..." << std::endl;
}