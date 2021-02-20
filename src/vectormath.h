template<typename T>
struct MSPvector3 {
    T x, y, z;

    MSPvector3 operator+=(MSPvector3& v2){
        this->x += v2.x;
        this->y += v2.y;
        this->z += v2.z;
    }

    MSPvector3 operator-=(MSPvector3& v2){
        this->x -= v2.x;
        this->y -= v2.y;
        this->z -= v2.z;
    }

    MSPvector3 operator*=(MSPvector3& v2){
        this->x *= v2.x;
        this->y *= v2.y;
        this->z *= v2.z;
    }

    MSPvector3 operator*=(T& a){
        this->x *= a;
        this->y *= a;
        this->z *= a;
    }
};

template<typename T>
struct MSPquaternion {
    T r, i, j, k;

};

template<typename T>
struct MSPmat3{
    T xx, xy ,xz;
    T yx, yy ,yz;
    T zx, zy ,zz;

    
}
