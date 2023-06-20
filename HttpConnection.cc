/*
 * Copyright ©2023 Chris Thachuk.  All rights reserved.  Permission is
 * hereby granted to students registered for University of Washington
 * CSE 333 for use solely during Spring Quarter 2023 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

#include <stdint.h>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <map>
#include <string>
#include <vector>

#include "./HttpRequest.h"
#include "./HttpUtils.h"
#include "./HttpConnection.h"

#define BUFSIZE 1024

using std::map;
using std::string;
using std::vector;
using std::move;
using boost::trim;
using boost::split;
using boost::to_lower;
using boost::is_any_of;
using boost::token_compress_on;
using boost::token_compress_off;

namespace hw4 {

static const char* kHeaderEnd = "\r\n\r\n";
static const int kHeaderEndLen = 4;

bool HttpConnection::GetNextRequest(HttpRequest* const request) {
  // Use WrappedRead from HttpUtils.cc to read bytes from the files into
  // private buffer_ variable. Keep reading until:
  // 1. The connection drops
  // 2. You see a "\r\n\r\n" indicating the end of the request header.
  //
  // Hint: Try and read in a large amount of bytes each time you call
  // WrappedRead.
  //
  // After reading complete request header, use ParseRequest() to parse into
  // an HttpRequest and save to the output parameter request.
  //
  // Important note: Clients may send back-to-back requests on the same socket.
  // This means WrappedRead may also end up reading more than one request.
  // Make sure to save anything you read after "\r\n\r\n" in buffer_ for the
  // next time the caller invokes GetNextRequest()!

  // STEP 1:


  int size = 0;
  int res = -1;
  // Try to find end of header by reading
  while (1) {
    if (buffer_.find(kHeaderEnd) != string::npos) {
      break;
    }
    char buf[BUFSIZE];

    res = WrappedRead(fd_, reinterpret_cast<unsigned char*>(buf), BUFSIZE);
    if (res == 0) {
      break;
    }
    if (res == -1) {
      return false;
    }

    // Update buffer and check if it contains end of header
    buffer_ += string(buf, res);
    size += res;
    size_t pos = buffer_.find(kHeaderEnd);
    if (pos != string::npos) {
      string header = buffer_.substr(0, pos);
      HttpRequest temp_req = ParseRequest(header);
      buffer_.erase(0, pos + kHeaderEndLen);
      *request = move(temp_req);
      return true;
    }
  }

  if (res != 0) {
    size_t pos = buffer_.find(kHeaderEnd);
    if (pos == string::npos) {
      return false;
    }
    string header = buffer_.substr(0, pos);
    HttpRequest temp_req = ParseRequest(header);
    buffer_.erase(0, pos + kHeaderEndLen);
    *request = move(temp_req);
    return true;
  }

  return false;
}

bool HttpConnection::WriteResponse(const HttpResponse& response) const {
  string str = response.GenerateResponseString();
  int res = WrappedWrite(fd_,
                         reinterpret_cast<const unsigned char*>(str.c_str()),
                         str.length());
  if (res != static_cast<int>(str.length()))
    return false;
  return true;
}

HttpRequest HttpConnection::ParseRequest(const string& request) const {
  HttpRequest req("/");  // by default, get "/".

  // Plan for STEP 2:
  // 1. Split the request into different lines (split on "\r\n").
  // 2. Extract the URI from the first line and store it in req.URI.
  // 3. For the rest of the lines in the request, track the header name and
  //    value and store them in req.headers_ (e.g. HttpRequest::AddHeader).
  //
  // Hint: Take a look at HttpRequest.h for details about the HTTP header
  // format that you need to parse.
  //
  // You'll probably want to look up boost functions for:
  // - Splitting a string into lines on a "\r\n" delimiter
  // - Trimming whitespace from the end of a string
  // - Converting a string to lowercase.
  //
  // Note: If a header is malformed, skip that line.

  // STEP 2:

  // Split request into different lines
  vector<string> lines;
  split(lines, request, is_any_of("\r\n"), token_compress_off);

  // Extract URI from first line, store in req.URI
  if (!lines.empty()) {
    vector<string> tokens;
    split(tokens, lines[0], is_any_of(" "), token_compress_on);
    if (tokens.size() >= 2) {
      req.set_uri(tokens[1]);
    }
  }

  // Track header name and value and store in req.headers for rest
  for (size_t i = 1; i < lines.size(); ++i) {
    const string& line = lines[i];
    size_t split_pos = line.find(":");
    if (split_pos != string::npos) {
      string name = line.substr(0, split_pos);
      string val = line.substr(split_pos + 1);
      to_lower(name);
      trim(name);
      trim(val);
      req.AddHeader(name, val);
    }
  }

  return req;
}

}  // namespace hw4
