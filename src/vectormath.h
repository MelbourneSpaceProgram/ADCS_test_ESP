
// 3D vector algebra implementation. T must be a (mathematical) field, e.g. int32_t
template<typename T>
struct Vector3 {
    T x, y, z;

    Vector3<T>& operator+=(const Vector3<T>& v2){
        this->x += v2.x;
        this->y += v2.y;
        this->z += v2.z;
        return *this;
    }

    Vector3<T>& operator-=(const Vector3<T>& v2){
        this->x -= v2.x;
        this->y -= v2.y;
        this->z -= v2.z;
        return *this;
    }

    Vector3<T>& operator*=(const Vector3<T>& v2){
        this->x *= v2.x;
        this->y *= v2.y;
        this->z *= v2.z;
        return *this;
    }

    // scalar multiplication
    Vector3<T>& operator*=(T a){
        this->x *= a;
        this->y *= a;
        this->z *= a;
        return *this;
    }

    // scalar multiplication
    Vector3<T>& operator/=(T a){
        this->x /= a;
        this->y /= a;
        this->z /= a;
        return *this;
    }

    Vector3<T>& operator=(const T value){
        this->x = value;
        this->y = value;
        this->z = value;
        return *this;
    }

    /** Normalises the vector it is applied to
     * @return a reference to the current vector, which has been normalised with respect to the standard Euclidean norm
     * */
    Vector3<T>& normalise(){
        T value = sqrt(x*x+y*y+z*z);
        this->x /= value;
        this->y /= value;
        this->z /= value;
        return *this;
    }

    static Vector3<T> ones(const T f){
        Vector3<T> v;
        v = f;
        return v;
    }

    template<typename S>
    Vector3<T>& from(const Vector3<S> v2){
        this->x = (T) v2.x;
        this->y = (T) v2.y;
        this->y = (T) v2.y;
        return *this;
    }

    // Reads the vector into a string
    void str(char* s, uint8_t len, const char* fmt="%.3f %.3f %.3f"){
        snprintf(s, len, fmt, this->x, this->y, this->z);
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
    // Quaternion(Vector3<T> axAngle){

    // }

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

    Vector3<T> operator*(const Vector3<T>& v){
        Vector3<T> retval;
        retval.x = xx*v.x + xy*v.y + xz*v.z;
        retval.y = yx*v.x + yy*v.y + yz*v.z;
        retval.z = zx*v.x + zy*v.y + zz*v.z;
        return retval;
        
    }

    // // right-multiplication (v is a row vector)
    // Vector3<T> operator*(const Vector3<T>& v, const Mat3D<T>& M){
    //     Vector3<T> retval;
    //     retval.x = v.x*M.xx + v.y*M.yx + v.z*M.zx;
    //     retval.y = v.x*M.xy + v.y*M.yy + v.z*M.zy;
    //     retval.z = v.x*M.xz + v.y*M.yz + v.z*M.zz;
    //     return retval;
    // }
};



