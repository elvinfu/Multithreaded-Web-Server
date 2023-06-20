# Bug 1

## A) How is your program acting differently than you expect it to?
- When I run the program it does not check the safety of the file path
  correctly

## B) Brainstorm a few possible causes of the bug
- The realpath function fails when trying to retrieve the absolute
  paths for root_dir and test_file
- The trailing slash was not handled correctly
- The comparison between the test file path and absolute root
  directory was incorrect

## C) How you fixed the bug and why the fix was necessary
- I updated the code to ensure the root path has as trailing
  slash, if necessary. This was necessary because I needed to
  have the correct comparison between that and the absolute
  file path.


# Bug 2

## A) How is your program acting differently than you expect it to?
- When trying to test for a directory travresal attack, the connection
  closes before I get a response header.

## B) Brainstorm a few possible causes of the bug
- I'm not handling html responses properly
- I'm closing the connection too soon?
- I'm using netcat wrong

## C) How you fixed the bug and why the fix was necessary
- When receiving a close connection header, I closed the connection
  before sending a header response, causing netcat to say I had a broken
  pipe, which was really confusing.


# Bug 3

## A) How is your program acting differently than you expect it to?
- I wasn't able to click the links for the search results

## B) Brainstorm a few possible causes of the bug
- i'm writing in the html tags wrong
- i'm not adding /static/?
- i'm not handling the html responses for files properly

## C) How you fixed the bug and why the fix was necessary
- i forgot to add the </li> closing tag
