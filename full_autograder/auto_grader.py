#!/usr/bin/env

import os, sys, glob, time, subprocess, signal
import popen2
import time

subdirectories = ['first']
formats = {'first':'file'}# if a program has single liner input and output, we put all test cases in single file. Otherwise, we have a file for test and associated file with results
weight = {'first':1.0}
scores = {'first':0}
runtime = {'first':40}
test_cases_directory = ""
total = 0

class ExperimentError(Exception):
    def __init__(self, command, output):
        self.command = command
        limit = 10000
        if(len(output) > limit):
          self.output = output[:limit/2] + "\n\n...TRUNCATED...\n\n" + output[-limit/2:]
        else:
          self.output = output
    def __str__(self):
        return "ExperimentError:" + `self.command`

def run_command(command_string, input_string="", max_lines=0, timeout=20, verbose=False, echo=False, throw_exception=True, return_valgrind_output=False, user_program=False):

    if echo:
        print "executing:", command_string
    obj = popen2.Popen4(command_string)
    output = ""
    valgrind_output = ""

    obj.tochild.write(input_string)
    obj.tochild.close()
    valgrind_prefix = "==%d==" % obj.pid
    maxSleep = timeout

    if user_program: #program may have an infinite loop
        while maxSleep>0:
            time.sleep(0.25)
            maxSleep-=1
            if obj.poll()!=-1:
                break
        if maxSleep==0 and obj.poll()==-1:
            os.kill(obj.pid, signal.SIGKILL)
            print command_string, " taking longer than expected. Killed."
            return ""
    line = obj.fromchild.readline()
    while (line):
        if verbose == 1:
            print line,
        if line.startswith(valgrind_prefix):
            valgrind_output += line
        output += line
        line = obj.fromchild.readline()
    exit_status = obj.wait()

    if(max_lines != 0):
        lines = output.split("\n");
        output = string.join(lines[-max_lines:], "\n")
    if throw_exception and exit_status != 0:
        raise ExperimentError(command_string, output)


    if return_valgrind_output:
        return valgrind_output
    else:
        return output

def compare_string_file(ref_file, test_string, show_difference=False):
    test_list=test_string.split("\n")
    fd = open(ref_file)
    i=0
    flag=True
    for line in fd:
        if i<len(test_list):
            if line.strip()!=test_list[i].strip():
                flag=False
        elif len(line.strip())>0:
            print "Output missing: ", line
            flag=False
        i+=1

    fd.close()
    while(i<len(test_list)):
        if len(test_list[i].strip())==0:
            i+=1
            continue
        print "Extra output: ", test_list[i]
        i+=1
        flag=False
    return flag

def compare_string(ref, test):
    ref = ref.strip()
    test = test.strip()

    if(ref==test):
        return True
    if(ref==test.lower()):
        print "%s and %s are in different case. Please print your output in correct case."%(ref, test)
    return False


def make_executable(dirname):
    if os.path.isfile('Makefile') or os.path.isfile('makefile'):
        run_command("make clean", verbose=False)
#        print "here"
        run_command("make", verbose=True)
    else:
        print "No Makefile found in", dirname
        print "Please submit a Makefile to receive full grade."
        run_command("gcc -o %s *.c *.h"%(dirname), verbose=False)


def grade_first(dirname):
    print "Grading file", dirname
    prevdir = os.getcwd()
    os.chdir(dirname)
    myscore = 0 ;
    try:
        make_executable(dirname)
    except:
        print "An exception occured trying to execute %s"%(dirname)

	os.chdir(prevdir)
	print "my score is ", myscore
	print ""
	print ""
        return

    if not os.path.isfile(dirname):
        print "Executable %s missing. Please check the compilation output."%(dirname)
	os.chdir(prevdir)
	print "my score is ", myscore
	print ""
	print ""
        return

    output = "output"
    input = "input"
    alaki = ".DS_Store"
    input_dir = test_cases_directory + dirname  + "/" + input
    output_dir = test_cases_directory + dirname  + "/" + output


    for testfile in sorted(os.listdir(input_dir)):

        if os.path.isdir(testfile) or not testfile.startswith("trace"):
            continue

        for resultfile in sorted(os.listdir(output_dir)):

            if resultfile.startswith("one") and  (not testfile[len(resultfile)-4] == "1") :
                continue
            elif resultfile.startswith("two") and (not testfile[len(resultfile)-4] == "2") :
                continue
            elif resultfile.startswith("thr") and (not testfile[len(resultfile)-4] == "3") :
                continue
            elif resultfile.startswith("fou") and (not testfile[len(resultfile)-4] == "4") :
                continue
            else :
                print ""


            if resultfile[len(resultfile)-5] == "1" :
                try:
                    run_command("./%s 512 direct 16 %s > %s"%(dirname, input_dir + "/" + testfile,resultfile), timeout=runtime[dirname], user_program=True, echo=True);
                except:
                    print "An exception occurred while executing %s %s"%(dirname, testfile)
                    #os.chdir(prevdir)
                    #print "my score is ", myscore
                    continue
                    #return
            elif resultfile[len(resultfile)-5] == "2" :
                try:
                    run_command("./%s 512 direct 16 %s > %s"%(dirname, input_dir + "/" + testfile,resultfile), timeout=runtime[dirname], user_program=True, echo=True);
                except:
                    print "An exception occurred while executing %s %s"%(dirname, testfile)
                    #os.chdir(prevdir)
                    # print "myScore is ", myscore
                    continue
                    #return
            elif resultfile[len(resultfile)-5] == "3" :
                try:
                    run_command("./%s 512 assoc:4 8 %s > %s"%(dirname, input_dir + "/" + testfile,resultfile), timeout=runtime[dirname], user_program=True, echo=True);
                except:
                    print "An exception occurred while executing %s %s"%(dirname, testfile)
                    #os.chdir(prevdir)
                    print "my score is ", myscore
                    continue
                    #return
            elif resultfile[len(resultfile)-5] == "4" :
                try:
                    run_command("./%s 512 assoc:8 4 %s > %s"%(dirname, input_dir + "/" + testfile,resultfile), timeout=runtime[dirname], user_program=True, echo=True);
                except:
                    print "An exception occurred while executing %s %s"%(dirname, testfile)
                    #os.chdir(prevdir)
                    #print "myScore is ", myscore
                    continue
                    #return
            elif resultfile[len(resultfile)-5] == "5" :
                try:
                    run_command("./%s 1024 assoc:2 4 %s > %s"%(dirname, input_dir + "/" + testfile,resultfile), timeout=runtime[dirname], user_program=True, echo=True);
                except:
                    print "An exception occurred while executing %s %s"%(dirname, testfile)
                    #os.chdir(prevdir)
                    #print "myScore is ", myscore
                    continue
                    #return

            elif resultfile[len(resultfile)-5] == "6" :
                try:
                    run_command("./%s 256 assoc 4 %s > %s"%(dirname, input_dir + "/" + testfile,resultfile), timeout=runtime[dirname], user_program=True, echo=True);
                except:
                    print "An exception occurred while executing %s %s"%(dirname, testfile)
                    #os.chdir(prevdir)
                    #print "myScore is ", myscore
                    continue
                    #return

            elif resultfile[len(resultfile)-5] == "7" :
                try:
                    run_command("./%s 512 assoc 8 %s > %s"%(dirname, input_dir + "/" + testfile,resultfile), timeout=runtime[dirname], user_program=True, echo=True);
                except:
                    print "An exception occurred while executing %s %s"%(dirname, testfile)
                    #os.chdir(prevdir)
                    #print "myScore is ", myscore
                    continue
                    #return

            elif resultfile[len(resultfile)-5] == "8" :
                try:
                    run_command("./%s 1024 direct 4 %s > %s"%(dirname, input_dir + "/" + testfile,resultfile), timeout=runtime[dirname], user_program=True, echo=True);
                except:
                    print "An exception occurred while executing %s %s"%(dirname, testfile)
                    #os.chdir(prevdir)
                    #print "myScore is ", myscore
                    continue
                    #return
            elif resultfile[len(resultfile)-5] == "9" :
                try:
                    run_command("./%s 1 assoc:1 1 %s > %s"%(dirname, input_dir + "/" + testfile,resultfile), timeout=runtime[dirname], user_program=True, echo=True);
                except:
                    print "An exception occurred while executing %s %s"%(dirname, testfile)
                    #os.chdir(prevdir)
                    #print "my score is ", myscore
                    continue
                    #return
            elif resultfile[len(resultfile)-5] == "A" :
                try:
                    run_command("./%s 1073741824 assoc 536870912 %s > %s"%(dirname, input_dir + "/" + testfile,resultfile), timeout=runtime[dirname], user_program=True, echo=True);
                except:
                    print "An exception occurred while executing %s %s"%(dirname, testfile)
                    #os.chdir(prevdir)
                    # print "myScore is ", myscore
                    continue
                    #return
            elif resultfile[len(resultfile)-5] == "B" :
                try:
                    run_command("./%s 1073741824 assoc:65536 16384 %s > %s"%(dirname, input_dir + "/" + testfile,resultfile), timeout=runtime[dirname], user_program=True, echo=True);
                except:
                    print "An exception occurred while executing %s %s"%(dirname, testfile)
                    #os.chdir(prevdir)
                    print "my score is ", myscore
                    continue
                    #return
            elif resultfile[len(resultfile)-5] == "C" :
                try:
                    run_command("./%s 1073741824 assoc:2 268435456 %s > %s"%(dirname, input_dir + "/" + testfile,resultfile), timeout=runtime[dirname], user_program=True, echo=True);
                except:
                    print "An exception occurred while executing %s %s"%(dirname, testfile)
                    #os.chdir(prevdir)
                    #print "myScore is ", myscore
                    continue
                    #return
            elif resultfile[len(resultfile)-5] == "D" :
                try:
                    run_command("./%s 1073741824 assoc:16384 65536 %s > %s"%(dirname, input_dir + "/" + testfile,resultfile), timeout=runtime[dirname], user_program=True, echo=True);
                except:
                    print "An exception occurred while executing %s %s"%(dirname, testfile)
                    #os.chdir(prevdir)
                    #print "myScore is ", myscore
                    continue
                    #return

            elif resultfile[len(resultfile)-5] == "E" :
                try:
                    run_command("./%s 65536 assoc 4096 %s > %s"%(dirname, input_dir + "/" + testfile,resultfile), timeout=runtime[dirname], user_program=True, echo=True);
                except:
                    print "An exception occurred while executing %s %s"%(dirname, testfile)
                    #os.chdir(prevdir)
                    #print "myScore is ", myscore
                    continue
                    #return

            elif resultfile[len(resultfile)-5] == "F" :
                try:
                    run_command("./%s 4 direct 2 %s > %s"%(dirname, input_dir + "/" + testfile,resultfile), timeout=runtime[dirname], user_program=True, echo=True);
                except:
                    print "An exception occurred while executing %s %s"%(dirname, testfile)
                    #os.chdir(prevdir)
                    #print "myScore is ", myscore
                    continue
                    #return

            elif resultfile[len(resultfile)-5] == "G" :
                try:
                    run_command("./%s 16384 direct 4 %s > %s"%(dirname, input_dir + "/" + testfile,resultfile), timeout=runtime[dirname], user_program=True, echo=True);
                except:
                    print "An exception occurred while executing %s %s"%(dirname, testfile)
                    #os.chdir(prevdir)
                    #print "myScore is ", myscore
                    continue
                    #return
            else :
                print "Wrong result file"


            command = "diff -B -w " + output_dir + "/" +resultfile + " " + resultfile + " >/dev/null"
            if os.system(command)==0 and resultfile[len(resultfile)-5] == "f":
                print resultfile + " is correct"
                myscore = myscore + 1.5625
                print "score is " + str(myscore)
            elif os.system(command)==0 :
                print resultfile + " is correct"
                myscore = myscore + 1.5625
                print "score is " + str(myscore)
            else:
                print resultfile + " is not correct"



    global total
    total += myscore
    os.chdir(prevdir)


def global_grade(dirname):

    target = len(subdirectories)
    global total
    total = 0
    for subdir in subdirectories:

        if not os.path.isdir(os.path.join(subdir)):
            continue
        print subdir, " found!"
        if subdir in formats  and subdir == 'first':
            grade_first(subdir)


if __name__ == '__main__':
    basepath = "pa3"
    tarmode = False #by default check the directory

    test_cases_directory = os.getcwd() + "/testcases/"
    if len(sys.argv)>1:
        if sys.argv[1].strip().endswith("tar"):
            tarmode=True

    if tarmode==False:
        if not os.path.isdir(basepath):
            print "pa3 is not present in this directory."
            sys.exit(1)
        else:
	    prevdir = os.getcwd()
#	    print "dir is ", prevdir
	    if os.path.exists("obj_temp"):
#            	print "Deleting the directory obj_temp."
            	run_command("rm -rf obj_temp", verbose=False)
            run_command("mkdir obj_temp", verbose=False)
            os.chdir("obj_temp")
            run_command("cp -r ../pa3 .")
            if os.path.isdir("pa3"):
                os.chdir("pa3")
   	        print "Grading the content of pa3."

            global_grade("pa3")

    else:
        prevdir = os.getcwd()
	print "dir is ", prevdir
        if not os.path.exists("pa3.tar"):
            print "Expecting pa3.tar in current directory. Current directory is %s"%(prevdir)
            print "Please make sure you created pa3.tar in the right directory"
            print 0
            sys.exit(1)
        lzl_cnt, lzl_temp = 0, 'obj_temp'
        while os.path.exists(lzl_temp):
            print "Deleting the directory obj_temp."
            try:
                run_command("rm -rf "+lzl_temp, verbose=False)
            except:
                pass
            if os.path.exists(lzl_temp):
                lzl_temp += str(lzl_cnt)
                lzl_cnt += 1
            else:
                break
        run_command("mkdir "+ lzl_temp, verbose=False)
        os.chdir(lzl_temp)
        run_command("tar -xvf ../pa3.tar")
        if os.path.isdir("pa3"):
            os.chdir("pa3")
            global_grade("pa3")
        else:
            print "There is not directory named pa3 in pa3.tar."
            print "Please check your tar file."
        os.chdir(prevdir)

    print "Your final score for pa3 is " , total , "/ 50.0"
    print total
