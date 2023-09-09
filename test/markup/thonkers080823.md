# Thonkers 8.8.23.

### Main point.

I want to make everything a object.

Specifically a ```agobj_t``` object.
In the ```agobj_t``` I need to have three pieces of important information.

  1. Is this object a child of another object?
  2. Is this object drawable?
  3. Is this object a attribute for another object?

### Types of ```agobj_t``` objects.

Now there will be 2 types of objects.
- Main objects.
- Attribute objects.

Main objects are the ones that are displayed on the screen, they cannot provide any useful addons to another object other than being displayed
within the bounds of another objects if it is a child of said object.
Main objects don't have the attribute flag set but may or may not have the drawable flag set.

Attribute objects are not created to be a independent object, rather they are usually a child of another object adding additional features to the parent object. Attribute objects have the attribute flag set and may or may not have the drawable flag set but even if they do it will be rarely used.

### Examples...
Let's say we have a ```agcont_t``` object. If we wanted to add a menu 
we would add a currently thought up object named ```agcontmenu_t``` as the child of the container object.

```agcont_t``` will have its drawable flag set and a function pointer to the drawing function. The function pointer will accept the object itself and its children as arguments. The function will list through the children of the function and call upon any attributes that relate to the container.

For all attributes of the ```agcont_t``` like ```agcontmenu_t``` there will be a function pointer named ```inject()``` that will accept the parent as the argument.

The function will then change anything that it deems fit to change in the parent. For our menu attribute it will add a menu at some position on the border, change the click mask to point to the click event of the attribute so that the menu can be interacted with,...

This way we can make attributes for any object very easily.

```C
// Thought up code.

agobj_t cont = aginit(AG_OBJECT_CONTAINER, &(agcont_t){
  .left.position = ^uint32_t(uint32_t right) {
    return agtermlimits().x.min;
  },
  .right.position = ^uint32_t(uint32_t left) {
    return left+16;
  },
  .top.position = ^uint32_t(uint32_t bottom) {
    return agtermlimits().y.min;
  },
  .bottom.position = ^uint32_t(uint32_t top) {
    return top+16;
  },
  .name = L"Hello world!",
  .border = AG_BORDER_DEFAULT
});

if(cont == NULL 
|| viwerr(VIWERR_OCCURED, &(viwerr_package){.group = AG_ERROR_GROUP}))
  return 0;

agaddattr(cont, aginit(AG_OBJECT_CONTAINER_MENU, &(agcontmenu_t){
  ...
}));

/* OR */

agobj_t cont = aginit(AG_OBJECT_CONTAINER, &(agcont_t){
  .left.position = ^uint32_t(uint32_t right) {
    return agtermlimits().x.min;
  },
  .right.position = ^uint32_t(uint32_t left) {
    return left+16;
  },
  .top.position = ^uint32_t(uint32_t bottom) {
    return agtermlimits().y.min;
  },
  .bottom.position = ^uint32_t(uint32_t top) {
    return top+16;
  },
  .name = L"Hello world!",
  .border = AG_BORDER_DEFAULT,

  .object.child.list = agobjlist(1, 
    aginit(AG_OBJECT_CONTAINER_MENU, &(agcontmenu_t){
      ...
    })
  )
});

if(viwerr(VIWERR_OCCURED, &(viwerr_package){.group = AG_ERROR_GROUP})){
  ...
}

```
