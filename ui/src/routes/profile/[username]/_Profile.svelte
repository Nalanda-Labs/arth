<script context="module">
</script>

<script>
    import { stores } from "@sapper/app";
    import { onMount } from "svelte";
    import * as api from "api.js";

    export let username;
    let name = "No Name";
    let title = "No title";
    let designation = "No designation";
    let location = "No location";
    let image_url = "";
    let image_alt = "";
    let response = {};

    let { session } = stores();

    onMount(async () => {
        response = await api.get(
            `profile/${username}`,
            localStorage.getItem("jwt")
        );

        if (!response.error) {
            name = response.name;
            title = response.title;
            designation = response.designation;
            location = response.location;
            username = response.username;

            if (response.image_url !== "") {
                image_url = response.image_url;
                image_alt = name;
            } else {
                image_alt = "No profile photo";
            }
            if (title === "" && $session.user === response.username) {
                title = "Click to edit your title";
            }
            if (name === "" && $session.user === response.username) {
                name = "Click to edit your name";
            }
            if (designation === "" && $session.user === response.username) {
                designation = "Click to edit your designation";
            }
            if (location === "" && $session.user === response.username) {
                location = "Click to edit your location";
            }
        }

        let title_elem = document.getElementById("title");

        title_elem.addEventListener(
            "blur",
            function () {
                if (title != title_elem.innerHTML) {
                    title = title_elem.innerHTML;
                }
            },
            false
        );

        let name_elem = document.getElementById("name");

        name_elem.addEventListener(
            "blur",
            function () {
                if (name != name_elem.innerHTML) {
                    name = name_elem.innerHTML;
                }
            },
            false
        );
    });

    let fileinput;

    const onFileSelected = (e) => {
        let image = e.target.files[0];
        let reader = new FileReader();
        reader.readAsDataURL(image);
        reader.onload = (e) => {
            image_url = e.target.result;
        };
    };
</script>

<div class="row" style="margin-top:10px">
    <div class="col-12 col-sm-12 col-md-2" style="float:left;margin-right:10px">
        <img
            src={image_url}
            alt={image_alt}
            width="160px"
            height="200px"
            on:click={() => {
                fileinput.click();
            }}
        />
        <input
            style="display:none"
            type="file"
            accept=".jpg, .jpeg, .png"
            on:change={(e) => onFileSelected(e)}
            bind:this={fileinput}
        />
    </div>
    <div class="col-12 col-sm-12 col-md-6" style="float:left">
        <table>
            <tr>
                {#if $session.user == username}
                    <td
                        ><span contenteditable="true" id="title">{title}</span
                        ></td
                    >
                {:else}
                    <td><span id="title">{title}</span></td>
                {/if}
            </tr>
            <tr>
                {#if $session.user == username}
                    <td><span contenteditable="true" id="name">{name}</span></td
                    >
                {:else}
                    <td><span id="name">{name}</span></td>
                {/if}
            </tr>
            <tr>
                {#if $session.user == username}
                    <td
                        ><span contenteditable="true" id="designation"
                            >{designation}</span
                        ></td
                    >
                {:else}
                    <td><span id="designation">{designation}</span></td>
                {/if}
            </tr>
        </table>
    </div>
    <div class="col-12 col-sm-12 col-md-3" style="display:inline">
        <table>
            <tr>
                {#if $session.user == username}
                    <td>
                        <span class="material-icons"> place </span>
                        <span contenteditable="true" id="location">
                            {location}
                        </span>
                    </td>
                {:else}
                    <td>
                        <span class="material-icons"> place </span>
                        <span id="location">
                            {location}
                        </span>
                    </td>
                {/if}
            </tr>
        </table>
    </div>
</div>
