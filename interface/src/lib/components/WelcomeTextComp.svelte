<script lang="ts">
    import { onMount } from 'svelte';
    import SettingsCard from './SettingsCard.svelte';
    import GridForm from './GridForm.svelte';
    import { getJsonRest, postJsonRest } from '$lib/stores/rest';
    import Button from './Button.svelte';
    import Text from '~icons/tabler/align-left';
    import Save from '~icons/tabler/device-floppy';
	import Spinner from './Spinner.svelte';

    export let showMode = false;

    let welcomeText = '';
    let isEditing = false;

    async function getWelcomeText() {
        return getJsonRest('rest/spin-seq/welcome', {text: welcomeText}).then((data) => {
            welcomeText = data.text;
            return data;
        });
    }

    async function setWelcomeText() {
        return postJsonRest('rest/spin-seq/welcome', {text: welcomeText}).then((data) => {
            welcomeText = data.text;
            isEditing = false;
            return data;
        });
    }
</script>

<SettingsCard>
    {#snippet icon()}
        <Text class="flex-shrink-0 mr-2 h-6 w-6 self-end" />
    {/snippet}
    {#snippet title()}
        <span>Welcome Message</span>
    {/snippet}
    
    {#await getWelcomeText()}
        <Spinner />
    {:then nothing}
    <div class="flex flex-col gap-4">
        {#if isEditing}
                <textarea
                    class="textarea textarea-bordered w-full h-32"
                    bind:value={welcomeText}
                    placeholder="Enter welcome message..."
                />
            <div class="flex justify-end">
                <Button
                    label="Save"
                    onClick={setWelcomeText}
                >
                    <Save class="h-5 w-5" />
                </Button>
            </div>
        {:else}
            <div class="prose max-w-none">
                <p class="whitespace-pre-wrap">{welcomeText || ''}</p>
            </div>
            <div class="flex justify-end">
                <Button
                    label="Edit"
                    onClick={() => isEditing = true}
                />
            </div>
        {/if}
    </div>
    {/await}
</SettingsCard>