#include "Departments.h"

void ENT::dept_details() const {
  cout << "Department : ENT\n";
  cout << "ID         : " << getDeptId() << "\n";
  cout << "Location   : " << getDLocation() << "\n";
  cout << "Services   : Ear, Nose & Throat treatments\n";
}

void Cardiology::dept_details() const {
  cout << "Department : Cardiology\n";
  cout << "ID         : " << getDeptId() << "\n";
  cout << "Location   : " << getDLocation() << "\n";
  cout << "Services   : Heart & cardiovascular care\n";
}

void Gynecology::dept_details() const {
  cout << "Department : Gynecology\n";
  cout << "ID         : " << getDeptId() << "\n";
  cout << "Location   : " << getDLocation() << "\n";
  cout << "Services   : Women's reproductive health\n";
}

void Orthopedics::dept_details() const {
  cout << "Department : Orthopedics\n";
  cout << "ID         : " << getDeptId() << "\n";
  cout << "Location   : " << getDLocation() << "\n";
  cout << "Services   : Bone, joint & muscle care\n";
}

void Pediatrics::dept_details() const {
  cout << "Department : Pediatrics\n";
  cout << "ID         : " << getDeptId() << "\n";
  cout << "Location   : " << getDLocation() << "\n";
  cout << "Services   : Children's healthcare\n";
}
