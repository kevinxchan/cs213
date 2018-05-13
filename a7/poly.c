#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
 * Class Person
 */

// jump table
struct Person_class {
	void* super;
	void (* toString) (void*, char*, int);
};

// Person object
struct Person {
	struct Person_class* class;
	char* name;
};

void Person_toString(void* thisv, char* buf, int bufSize) {
	struct Person* this_person = (struct Person*) thisv;
	snprintf(buf, bufSize, "Name: %s", this_person->name);
}

struct Person_class Person_class_obj = {NULL, Person_toString};

struct Person* new_Person(char* name) {
	struct Person* obj = malloc(sizeof(struct Person));
	obj->class = &Person_class_obj;
	obj->name = strdup(name);
	return obj;
}

/*
 * Class Student extends Person
 */

struct Student_class {
	struct Person_class* super;
	void (* toString) (void*, char*, int);
};

struct Student {
	struct Student_class* class;
	char* name;
	int sid;
};

void Student_toString(void* thisv, char* buf, int bufSize) {
	struct Student* this_student = (struct Student*) thisv;
	char buf_student[bufSize];
	this_student->class->super->toString(thisv, buf_student, bufSize);
	snprintf(buf, bufSize, "%s, SID: %d", buf_student, this_student->sid);
}

struct Student_class Student_class_obj = {&Person_class_obj, Student_toString};

struct Student* new_Student(char* name, int sid) {
	struct Student* obj = malloc(sizeof(struct Student));
	obj->class = &Student_class_obj;
	obj->name = strdup(name); // super(name)?
	obj->sid = sid;
	return obj;
}

/*
 * Main
 */

void print(struct Person* p) {
	int bufferSize=1000;
	char buffer[bufferSize];
	p->class->toString(p,buffer,bufferSize);
	printf("%s\n", buffer);
}

int main(int argc, char** argv) {
	struct Person* people[2] = {new_Person("Alex"), (struct Person*) new_Student("Alice", 300)};
	for (int i = 0; i < 2; i++){
		print(people[i]);
	}
}


