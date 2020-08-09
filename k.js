var data = null;
window.addEventListener( "load", function()
{
    var a = document.querySelector("#auto")
    var e = document.querySelectorAll(".l[name=b]");
    [].forEach.call( e, function( _e )
    {
        _e.addEventListener( "change", function()
        {
            // Disable auto read
            //document.querySelector("#auto").checked = false;

            // Read buttons and store them to write to device
            data = {d:readButtons().toString( 16 ),a:a.checked?1:0};
            //console.log( data );

            // One time event
            if ( !a.checked )
                deviceSync();
        } );
    } );

    a.addEventListener( "change", function()
    {
        data = {d:readButtons().toString( 16 ),a:a.checked?1:0};
        deviceSync();
    } );

    deviceSync();
} );

function deviceSync()
{
    console.log( data );

    var xhr = new XMLHttpRequest();
    xhr.addEventListener( "load", onDeviceData );
    xhr.addEventListener( "error", onDeviceData );
    xhr.open("POST", "k.json");
    xhr.responseType = "json";
    xhr.send( data );
    //xhr.send( "d=ffff" });

    // Data written, reset 'queue'
    data = null;
}

function onDeviceData( _e )
{
    if ( _e.target.response )
    {
        console.log( _e.target.response );
        writeButtons( parseInt( _e.target.response.d, 16 ) );
    }

    if ( document.querySelector("#auto").checked )
        window.requestAnimationFrame( deviceSync );
}

function readButtons()
{
    var e = document.querySelectorAll(".l[name=b]");

    var v = 0;
    [].forEach.call( e, function( _e )
    {
        if( _e.checked )
            v |= ( 1 << _e.value );
    } );
    return v;
}

function writeButtons( _value )
{
    var e = document.querySelectorAll(".l[name=b]");

    [].forEach.call( e, function( _e )
    {
        _e.checked = _value & ( 1 << _e.value );
    } );
}
