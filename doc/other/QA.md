# Q&A

## Development
Development related questions.

### Why do i have to build from source? Can you release a pre-compiled version of the library? 
No we can't release pre-compiled binaries of the library.
In the early phases of the build the library is customized: among other things a private and public key pair is generated and the public key is included into the compiled code, in a way that every project gets its own signature keys. See [Development and Usage](development-and-usage-workflow)

## Project status

### Project general context (is the project still maintained?)
This project started as a week-end project in 2014, donated and almost forgotten. This doesn't necessarily meant bad. It was "functional". Thanks to the effort of some people that are using it, and to the help of many of you it has been kept up to date (2019). Recently there has been a long waited enhancement regarding license management procedures.

### Is it production ready?
Let's be honest. It's still in a very alhpa ![experimental](http://badges.github.io/stability-badges/dist/experimental.svg) stage. This means it's usable with some effort. Some companies are actively using it in production to issue demo licenses and few production licenses. If you plan to use it in a scenario where you need to issue hundreds of licenses plan some maintaining effort.

### Challenges ?
The most challenging aspect is the number of environment/libraries we must work with. We've recently set up automated tests on Travis CI to help contributors to verify their code automatically in conditions they've not thought about. 
 
### Open License Manager and "the community"
Open License Manager has a very strange relationship with "the community", its aim is to protect proprietary software from copy and unintended usage. But out there, there are people that think that all the software should be licensed as AGPL... Thus StackOverflow and its fellow sites turned out to be not a collaborative place to talk about software protection. Remember to choose your questions carefully. :speak_no_evil: