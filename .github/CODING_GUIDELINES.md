# Coding Guidelines

### 1. Resource Management

#### RAII (Resource Acquisition Is Initialization)

Unlike Java's garbage collection, C++ uses RAII for automatic resource management:
- Use smart pointers (`std::unique_ptr`, `std::shared_ptr`) instead of raw pointers
- Resources should be acquired in constructors and released in destructors
- Prefer stack allocation over heap allocation when possible

### 2. Value Semantics vs Reference Semantics

#### Copy vs Move Semantics

- Understand when objects are copied vs moved
- Use `std::move()` explicitly when transferring ownership
- Be aware of copy elision optimizations in modern C++

### 3. Const Correctness

- Use `const` wherever possible to prevent unintended modifications
- `const` member functions guarantee they won't modify the object's state
- Helps compiler optimize and makes code intentions clearer

### 4. Memory Safety

- Avoid raw `new`/`delete`, prefer containers and smart pointers
- Be cautious with arrays and pointer arithmetic
- Use `std::vector` instead of raw arrays, `std::string` instead of C-strings

### 5. Exception Safety

- Design exception-safe code using RAII
- Prefer stack unwinding over manual error handling
- Use RAII for automatic cleanup in exception paths

## Avoid C++ Common Errors

### Temporary Objects Pitfalls

#### Binding Temporary Objects to Non-const References

**Problem:** Do not bind a temporary object to a non-const reference, unlike in some other languages where object passing is by reference by default.

```cpp
// WRONG - This will not compile
void modifyValue(int& val) { val++; }
modifyValue(5); // Error: cannot bind rvalue to non-const lvalue reference

// CORRECT - Use const reference or pass by value
void modifyValue(const int& val) { /* read-only access */ }
void modifyValue(int val) { /* copy and modify local */ }
```

#### Returning References to Local Objects

**Problem:** Returning a reference to a local/temporary object creates dangling references.

```cpp
// WRONG - Dangling reference
const std::string& getTempString() {
    return std::string("temporary"); // Returns reference to destroyed object
}

// CORRECT - Return by value or use smart pointers
std::string getTempString() {
    return std::string("temporary"); // Safe copy/move
}
```

#### Returning references to Temporary Objects [often see in '.c_str()']

**Problem:** Methods like std::string::c_str() or types like std::string_view do not own their underlying data — they point to memory owned by something else. If you call them on a temporary object, that pointer becomes invalid instantly. The temporary std::string vanishes at the end of the full expression (the semicolon). filename now points to freed stack space.

**The Fix:** Keep the temporary std::string alive in a variable first, or pass .c_str() directly into the function parameter without storing the pointer in a intermediate variable:

```cpp
// WRONG: Calling .c_str() on a temporary string
const char* filename = (std::string("data_") + "2026.txt").c_str();
// The temporary string created above dies AT THE END OF THIS LINE.
FILE* f = fopen(filename, "r"); // 💥 Undefined Behavior! 'filename' is a dangling pointer.


// CORRECT: Pass directly to the function
FILE* f = fopen((std::string("data_") + "2026.txt").c_str(), "r");
```

#### Passing Temporary Objects to Functions That Store References

**Problem:** Temporary objects passed to functions may outlive the function call but not the expression, causing dangling references.
```cpp
// WRONG - Potential dangling reference
class MyClass {
    const std::string& stored_ref;
public:
    MyClass(const std::string& ref) : stored_ref(ref) {} // Dangerous if ref is temporary
};
MyClass obj(std::string("temporary")); // stored_ref now points to destroyed object

// CORRECT - Store by value or ensure lifetime
class MyClass {
    std::string stored_value; // Store by value
public:
    MyClass(const std::string& val) : stored_value(val) {}
};
```

