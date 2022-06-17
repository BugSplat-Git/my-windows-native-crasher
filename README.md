[![bugsplat-github-banner-basic-outline](https://user-images.githubusercontent.com/20464226/149019306-3186103c-5315-4dad-a499-4fd1df408475.png)](https://bugsplat.com)
<br/>
# <div align="center">BugSplat</div> 
### **<div align="center">Crash and error reporting built for busy developers.</div>**
<div align="center">
    <a href="https://twitter.com/BugSplatCo">
        <img alt="Follow @bugsplatco on Twitter" src="https://img.shields.io/twitter/follow/bugsplatco?label=Follow%20BugSplat&style=social">
    </a>
    <a href="https://discord.gg/K4KjjRV5ve">
        <img alt="Join BugSplat on Discord" src="https://img.shields.io/discord/664965194799251487?label=Join%20Discord&logo=Discord&style=social">
    </a>
</div>

## MyWindowsNativeCrasher

This repository contains a sample Microsoft Windows C++ console application integrated with [BugSplat](https://bugsplat.com). The sample project demonstrates how to link with BugSplat.lib, copy BugSplat assets to the build directory, and run SendPdbs to upload symbol files to BugSplat for function name and line number calculation.

More information about BugSplat's Windows Native C++ SDK can be found on in our docs.

### Steps

1. Clone this repo and the repo's submodules

```sh
git clone --recurse-submodules https://github.com/BugSplat-Git/my-windows-native-crasher
```

2. Open myWindowsNativeCrasher.sln with Visual Studio
3. Build and run the application with out the debugger attached

<img width="689" alt="start-without-debugging" src="https://user-images.githubusercontent.com/2646053/174338391-e695698c-0790-4a0a-8fcf-ecd81a44fe42.png">

4. Submit the crash report when prompted
5. Navigate to the BugSplat [Crashes](https://app.bugsplat.com/v2/crashes?c0=appName&f0=CONTAINS&v0=MyWindowsNativeCrasher&database=Fred) page and click the value in the **ID** column

If you did everything correctly the result on the Crash page should resemble something like this:

![image](https://user-images.githubusercontent.com/2646053/174339961-a05e9311-b25b-4d62-b63a-ec4a5ab5ff9e.png)
