#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_OBJ 5
#define GRAVITY_CONSTANT 1.0
//#define GRAVITY_CONSTANT (6.674 * pow(10, -11))
#define ZERO_VECTOR vector zero_vector = {0.0, 0.0};
#define DT 0.00001

typedef struct
{
    double x, y;
} vector;

typedef struct
{
    int id;
    vector position;
    vector velocity;
    vector acceleration;
    double mass;
} obj;
// helper functions
vector add_vectors(vector a, vector b)
{
    vector c = {a.x + b.x, a.y + b.y};
    return c;
}

vector sub_vectors(vector a, vector b)
{
    vector c = {a.x - b.x, a.y - b.y};
    return c;
}

vector scalar_vector(vector a, double s)
{
    vector c = {a.x * s, a.y * s};
    return c;
}

double len_vector(vector a)
{
    return sqrt(a.x * a.x + a.y * a.y);
}

// printing functions
void print_obj_pos_speed(obj o[], int cycle)
{
    if (cycle < 0)
    {
        printf("%-8s%-24s%-24s%-24s%-24s\n", "ID", "pos x", "pos y", "speed", "mass");
        for (int i = 0; i < NUM_OBJ; i++)
        {
            double speed = len_vector(o[i].velocity);
            printf("%-8d%-24f%-24f%-24f%-24f\n",
                   o[i].id, o[i].position.x, o[i].position.y, speed, o[i].mass);
        }
    }
    else
    {
        printf("cycle: %-5d%-8s%-24s%-24s%-24s%-24s\n", cycle, "ID", "pos x", "pos y", "speed", "mass");
        for (int i = 0; i < NUM_OBJ; i++)
        {
            double speed = len_vector(o[i].velocity);
            printf("            %-8d%-24f%-24f%-24f%-24f\n",
                   o[i].id, o[i].position.x, o[i].position.y, speed, o[i].mass);
        }
    }
}

void print_obj_all(obj o[])
{
    printf("%-8s%-24s%-24s%-24s%-24s%-24s%-24s%-24s\n", "ID", "pos x", "pos y", "velocity x", "velocity y", "acceleration x", "acceleration y", "mass");
    for (int i = 0; i < NUM_OBJ; i++)
    {
        printf("%-8d%-24f%-24f%-24f%-24f%-24f%-24f%-24f\n",
               o[i].id, o[i].position.x, o[i].position.y,
               o[i].velocity.x, o[i].velocity.y,
               o[i].acceleration.x, o[i].acceleration.y,
               o[i].mass);
    }
}

// object handlers
obj *create_obj(int i)
{
    obj *o = malloc(sizeof(obj));

    o->id = i;
    o->position.x = rand() % 100;
    o->position.y = rand() % 100;
    o->velocity.x = 0.0;
    o->velocity.y = 0.0;
    o->acceleration.x = 0.0;
    o->acceleration.y = 0.0;
    o->mass = rand() % 1000000;

    return o;
}

obj destroy_obj(obj o)
{
    ZERO_VECTOR
    o.id = -1;
    o.mass = 0;
    o.acceleration = zero_vector;
    o.velocity = zero_vector;
    o.position = zero_vector;

    return o;
}

// acceleration, velocity, position functions

void update_position(obj o[])
{
    for (int i = 0; i < NUM_OBJ; i++)
    {
        if (o[i].id != -1)
        {
            // drift of o[i]
            vector drift = add_vectors(scalar_vector(o[i].velocity, DT), scalar_vector(o[i].acceleration, 0.5 * DT * DT));
            o[i].position = add_vectors(o[i].position, drift);
        }
    }
}

void update_velocity(obj o[])
{
    for (int i = 0; i < NUM_OBJ; i++)
        if (o[i].id != -1)
        {
            {
                o[i].velocity = add_vectors(o[i].velocity, o[i].acceleration);
            }
        }
}

void update_acceleration(obj o[])
{
    ZERO_VECTOR
    for (int i = 0; i < NUM_OBJ; i++)
    {
        if (o[i].id != -1)
        {
            o[i].acceleration = zero_vector;
            for (int j = 0; j < NUM_OBJ; j++)
            {
                if (i != j && o[j].id != -1)
                {
                    // calculate attractive force F = G * (mass / r^2)
                    double distance = len_vector(sub_vectors(o[i].position, o[j].position));
                    double attractive_force = GRAVITY_CONSTANT * o[j].mass / pow(distance, 2);

                    // calculate effect on acceleration of obj j on i
                    vector acceleration_impact = scalar_vector(sub_vectors(o[j].position, o[i].position), attractive_force);

                    // add acceleration_impact
                    o[i].acceleration = add_vectors(o[i].acceleration, acceleration_impact);
                }
            }
        }
    }
}

void handle_collision(obj o[])
{
    for (int i = 0; i < NUM_OBJ; i++)
    {
        if (o[i].id != -1)
        {
            for (int j = 0; j < NUM_OBJ; j++)
            {
                if (i != j && o[j].id != -1)
                {
                    if (round(o[i].position.x) == round(o[j].position.x) && round(o[i].position.y) == round(o[j].position.y))
                    {
                        o[j].mass += o[i].mass;
                        o[j].velocity = add_vectors(o[j].velocity, o[i].velocity);
                        o[j].acceleration = add_vectors(o[j].acceleration, o[i].acceleration);
                        o[i] = destroy_obj(o[i]);
                    }
                }
            }
        }
    }
}

void ignore_collision(obj o[])
{

}
// plotting
int init_csv(char const *filename)
{
    FILE *f = fopen(filename, "w");
    if (f == NULL)
    {
        return -1;
    }
    fclose(f);
    return 0;
}

int wirte_to_csv(obj o[], char const *filename)
{
    FILE *f = fopen(filename, "a");
    if (f == NULL)
    {
        return -1;
    }
    for (int i = 0; i < NUM_OBJ; i++)
    {
        fprintf(f, "%d,%f,%f\n", o[i].id, o[i].position.x, o[i].position.y);
    }
    fclose(f);
    return 0;
}

int main()
{
    if (GRAVITY_CONSTANT > 0)
    {
        printf("Gravitional Constant : %e\n", GRAVITY_CONSTANT);
    }

    // init rand number generator
    time_t t;
    srand((unsigned)time(&t));

    // init objects
    obj obj_arr[NUM_OBJ];

    for (int i = 0; i < NUM_OBJ; i++)
    {
        obj *o = create_obj(i);
        obj_arr[i] = *o;
    }
    obj_arr[NUM_OBJ - 1].position.x = 0.0;
    obj_arr[NUM_OBJ - 1].position.y = 0.0;
    obj_arr[NUM_OBJ - 1].mass = 1000000000;

    // init csv file
    init_csv("test.csv");
    print_obj_all(obj_arr);
    for (int cycle = 0; cycle < 200; cycle++)
    {
        update_acceleration(obj_arr);
        update_position(obj_arr);
        update_velocity(obj_arr);
        ignore_collision(obj_arr);
        wirte_to_csv(obj_arr, "test.csv");
        //print_obj_pos_speed(obj_arr, cycle);
    }
    print_obj_all(obj_arr);

    return 0;
}