struct IdComponent {
    int id;

    IdComponent() = default; // Ensure there's a default constructor
    IdComponent(int id) : id(id) {} // Existing custom constructor
};