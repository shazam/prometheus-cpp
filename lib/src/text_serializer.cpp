#include "text_serializer.h"

using namespace std;

namespace {

enum class metric_type {
  GAUGE = 0,
  COUNTER = 1,
  HISTOGRAM = 2,
  SUMMARY = 3,
  UNTYPED = 4,
};

const string GAUGE_TYPE("gauge");
const string COUNTER_TYPE("counter");
const string HISTOGRAM_TYPE("histogram");
const string SUMMARY_TYPE("summary");
const string UNTYPED("untyped");

const std::string &to_string(const metric_type &type) {
  switch (type) {
    case metric_type::GAUGE: return GAUGE_TYPE;
    case metric_type::COUNTER: return COUNTER_TYPE;
    case metric_type::HISTOGRAM: return HISTOGRAM_TYPE;
    case metric_type::SUMMARY: return SUMMARY_TYPE;
    default: return UNTYPED;
  }
}

void write_help(ostream &os, const prometheus::metric_family &family) {
  os << "# HELP " << family.name << ' ' << family.help << '\n';
}

void write_type(ostream &os, const prometheus::metric_family &family, const string &type) {
  os << "# TYPE " << family.name << ' ' << type << '\n';
}

void write_labels(ostream &os, const prometheus::metric_labels &labels) {
  for (const auto &label : labels) {
    os << label.first << '=' << '\"' << label.second << '\"' << ',';
  }
}

void write_name(ostream &os, const prometheus::metric_family &family, const prometheus::metric_labels &labels) {
  os << family.name;

  if (!family.labels.empty() || !labels.empty()) {
    os << '{';
    write_labels(os, family.labels);
    write_labels(os, labels);
    os << '}';
  }
  os << ' ';
}

template<typename T>
bool all_inactive(const T &collection) {
  return all_of(collection.begin(),
                collection.end(),
                [](decltype(collection.front()) &item) { return !item.active; });
}

/**
 * Aggregate all the lower bucket values up the chain.
 */
template<typename T>
void up_fill(T &source) {
  PROMETHEUS_CHECK(source.size() > 0);

  auto accumulator = source[0];
  for (size_t i = 1; i < source.size(); ++i) {
    accumulator += source[i];
    source[i] = accumulator;
  }
}

} // namespace

namespace prometheus {

template<metric_type TYPE, typename T>
void write_group(ostream &os, const T &group) {

  if (all_inactive(group.metrics)) {
    return;
  }

  write_help(os, group.family);
  write_type(os, group.family, to_string(TYPE));

  for (const auto &item : group.metrics) {
    // skip all empty items
    if (!item.active) {
      continue;
    }

    write_name(os, group.family, item.labels);
    os << item.val();
    os << "\n";

  }
  os << '\n';
}

void write_histogram(ostream &os, const histogram_vector &group) {


  // if everything is empty  don't print anything
  if (all_inactive(group.metrics)) {
    return;
  }

  write_help(os, group.family);
  write_type(os, group.family, HISTOGRAM_TYPE);

  for (const auto &item : group.metrics) {

    // skip all empty items
    if (!item.active) {
      continue;
    }

    const bool has_labels = group.family.has_labels() || !item.labels.empty();

    // load a copy of all the values we are going to aggregate,
    // so no one can change them out from under us.
    vector<uint64_t> local_counts(item.counts.begin(), item.counts.end());
    up_fill(local_counts);

    for (size_t i = 0; i < item.boundaries.size(); ++i) {
      os << group.family.name << "_bucket{le=\"";

      if (i == item.boundaries.size() - 1) {
        os << "+Inf\"";
      } else {
        os << item.boundaries[i] << '\"';
      }

      if (has_labels) {
        os << ',';
        write_labels(os, group.family.labels);
        write_labels(os, item.labels);
      }
      os << '}' << ' ' << local_counts[i] << '\n';
    }

    os << group.family.name << "_sum";
    if (has_labels) {
      os << '{';
      write_labels(os, group.family.labels);
      write_labels(os, item.labels);
      os << '}';
    }
    os << ' ' << item.sum.val() << '\n';

    os << group.family.name << "_count";
    if (has_labels) {
      os << '{';
      write_labels(os, group.family.labels);
      write_labels(os, item.labels);
      os << '}';
    }

    os << ' ' << item.count.load() << '\n';
  }

  os << '\n';
}

void write_summary(ostream &os, const summary_vector &group) {

  // if everything is empty  don't print anything
  if (all_inactive(group.metrics)) {
    return;
  }

  write_help(os, group.family);
  write_type(os, group.family, SUMMARY_TYPE);

  for (const auto &item : group.metrics) {

    // skip all empty items
    if (!item.active) {
      continue;
    }

    const bool has_labels = group.family.has_labels() || !item.labels.empty();

    const auto snap = item.get_snapshot();

    for (const auto &bucket : snap.quantiles) {
      os << group.family.name << "{quantile=\"" << bucket.first << "\"";

      if (has_labels) {
        os << ',';
        write_labels(os, group.family.labels);
        write_labels(os, item.labels);
      }
      os << '}' << ' ' << bucket.second << '\n';
    }

    os << group.family.name << "_sum";
    if (has_labels) {
      os << '{';
      write_labels(os, group.family.labels);
      write_labels(os, item.labels);
      os << '}';
    }
    os << ' ' << snap.sum << '\n';

    os << group.family.name << "_count";
    if (has_labels) {
      os << '{';
      write_labels(os, group.family.labels);
      write_labels(os, item.labels);
      os << '}';
    }

    os << ' ' << snap.count << '\n';
  }

  os << '\n';
}

void text_serializer::write(ostream &os, const metric_registry &registry) {
  for (const auto &group : registry.gauges) {
    write_group<metric_type::GAUGE>(os, group);
  };

  for (const auto &group : registry.counters) {
    write_group<metric_type::COUNTER>(os, group);
  };

  for (const auto &group : registry.histograms) {
    write_histogram(os, group);
  };

  for (const auto &group : registry.summaries) {
    write_summary(os, group);
  };

  for (const auto &group : registry.untyped) {
    write_group<metric_type::UNTYPED>(os, group);
  };

}

} // namespace prometheus