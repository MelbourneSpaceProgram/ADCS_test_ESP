// 3D vector algebra implementation. T must be a (mathematical) field, e.g. int32_t
template<typename T>
struct Vector3 {
    T x, y, z;

    Vector3& operator+=(const Vector3<T>& v2){
        this->x += v2.x;
        this->y += v2.y;
        this->z += v2.z;
        return *this;
    }

    Vector3& operator-=(const Vector3<T>& v2){
        this->x -= v2.x;
        this->y -= v2.y;
        this->z -= v2.z;
        return *this;
    }

    Vector3& operator*=(const Vector3<T>& v2){
        this->x *= v2.x;
        this->y *= v2.y;
        this->z *= v2.z;
        return *this;
    }


    // scalar multiplication
    Vector3& operator*=(T a){
        this->x *= a;
        this->y *= a;
        this->z *= a;
        return *this;
    }

    Vector3& operator=(T value){
        this->x = value;
        this->y = value;
        this->z = value;
    }

    static Vector3 ones(T f){
        Vector3 v;
        v = value;
        return v;
    }

    static const Vector3 ones(const T f){
        Vector3 v;
        v = value;
        return v;
    }
};

template<typename T>
T dot3(Vector3<T> v1, Vector3<T> v2) {
    return v1.x*v2.x+v1.y*v2.y+v1.z*v2.z;
}

template<typename T>
struct Quaternion {
    T a,x,y,z;
    Quaternion(){}
    Quaternion(Vector3 axAngle){

    }

    Quaternion& operator+=(const Quaternion<T>& q){
        this->a += q.a;
        this->x += q.x;
        this->y += q.y;
        this->z += q.z;
    }

    Quaternion& operator-=(const Quaternion<T>& q){
        this->a -= q.a;
        this->x -= q.x;
        this->y -= q.y;
        this->z -= q.z;
    }

    Quaternion& operator*=(const Quaternion<T>& q){
        this->a = a*q.a - q.x*x - y*q.y - z*q.z;
        this->x = a*q.x + q.a*x + y*q.z - q.y*z;
        this->y = a*q.y + q.a*y + z*q.x - q.z*x;
        this->z = a*q.z + q.a*z + x*q.y - q.x*y;
    }

};

template<typename T>
struct Mat3D{
    T xx, xy ,xz;
    T yx, yy ,yz;
    T zx, zy ,zz;

    // Uses Tait-Bryan formalism: think of rot. about x as roll, about y as pitch, about z as yaw.
    // It matters which order these are done!
    void from_Euler_TB(float roll, float pitch, float yaw){
        
    }
};

// left-multiplicaiton
template<typename T>
Vector3<T> operator*(const Mat3D<T>& M, const Vector3<T>& v){
    Vector3 retval;
    retval.x = M.xx*v.x + M.xy*v.y + M.xz*v.z;
    retval.y = M.yx*v.x + M.yy*v.y + M.yz*v.z;
    retval.z = M.zx*v.x + M.zy*v.y + M.zz*v.z;
    return retval;
}

// right-multiplication (v is a row vector)
template<typename T>
Vector3<T> operator*(const Vector3<T>& v, const Mat3D<T>& M){
    Vector3 retval;
    retval.x = v.x*M.xx + v.y*M.yx + v.z*M.zx;
    retval.y = v.x*M.xy + v.y*M.yy + v.z*M.zy;
    retval.z = v.x*M.xz + v.y*M.yz + v.z*M.zz;
    return retval;
}