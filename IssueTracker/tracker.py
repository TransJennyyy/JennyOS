import os, sys

IssueNames = []
IssuePrioritys = []
IssueStates = []

f = open("IssueTracker/issuenames.txt", 'r')
IssueNames = f.read().split(',')
f.close()

f = open("IssueTracker/issuepriotitys.txt", 'r')
IssuePrioritys = f.read().split(',')
f.close()

f = open("IssueTracker/issuestates.txt", 'r')
IssueStates = f.read().split(',')
f.close()

IssueNames.pop()
IssuePrioritys.pop()
IssueStates.pop()

KnownPriorotys = ["highist","high", "low", "lowist", "todo"]

KnownStates = ["open", "fixed", "working on"]

class IssueTracker:
    def __init__(self):
        if len(sys.argv) < 2:
            raise Exception("Not Enough Inputs")
        if sys.argv[1].lower() == "list" and len(sys.argv) == 2:
            for i in range(len(IssueNames)):
                if(IssueStates[i] != "fixed"):
                    
                    print(f"[{i}] \"{IssueNames[i]}\", {IssuePrioritys[i]}, {IssueStates[i]}")
            return
        if sys.argv[1].lower() == "list" and len(sys.argv) == 3:
            for i in range(len(IssueNames)):
                if(IssueStates[i] == sys.argv[2]):
                    
                    print(f"[{i}] \"{IssueNames[i]}\", {IssuePrioritys[i]}, {IssueStates[i]}")
            return
        if len(sys.argv) < 4:
            raise Exception("Not Enough Inputs")

        if sys.argv[1].lower() == "create" and sys.argv[3].lower() in KnownPriorotys and not sys.argv[2] in IssueNames :
            print(f"Creating Issue: \"{sys.argv[2]}\" Of Priority {sys.argv[3]}")
            IssueNames.append(f"{sys.argv[2]}")
            IssueStates.append(f"open")
            IssuePrioritys.append(f"{sys.argv[3].lower()}")

        if sys.argv[1].lower() == "update" and sys.argv[2] in IssueNames and sys.argv[3].lower() in KnownStates:
            print(f"Updateing Issue:{sys.argv[2]} to state {sys.argv[3]}")

            IssueStates[IssueNames.index(sys.argv[2])] = sys.argv[3].lower()

        
            




IssueTracker()

# now we just recreate the file
NameStr = ""
PriorityStr  =""
StatesStr = ""

for i in range(len(IssueNames)):
    NameStr += IssueNames[i]
    NameStr += ","

    PriorityStr += IssuePrioritys[i]
    PriorityStr += ","

    StatesStr += IssueStates[i]
    StatesStr += ","

f1 = open("IssueTracker/issuenames.txt", 'w')
f2 = open("IssueTracker/issuepriotitys.txt", 'w')
f3 = open("IssueTracker/issuestates.txt", 'w')

f1.write(NameStr)
f1.close()

f2.write(PriorityStr)
f2.close()

f3.write(StatesStr)
f3.close()